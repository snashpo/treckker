#include <stdio.h>
#include "stm32f10x.h"

#include "eeprom.h"

/*--------------------------------------------------
* uint16_t VirtAddVarTab[NumbOfVar] = {
*   RF1_OFFSET_L, RF1_OFFSET_H, CALIBRATED
* };
*--------------------------------------------------*/

uint16_t DataVar = 0;

/* Virtual address defined by the user: 0xFFFF value is prohibited */
/*--------------------------------------------------
* extern uint16_t VirtAddVarTab[NumbOfVar];
*--------------------------------------------------*/
uint16_t VirtAddVarTab[NumbOfVar] = {
  RF1_OFFSET_L, RF1_OFFSET_H, CALIBRATED
};

static FLASH_Status EE_Format(void);
static uint16_t EE_FindValidPage(uint8_t Operation);
static uint16_t EE_VerifyPageFullWriteVariable(uint16_t VirtAddress, 
					       uint16_t Data);
static uint16_t EE_PageTransfer(uint16_t VirtAddress, uint16_t Data);
static uint16_t EE_ReadVariable(uint16_t VirtAddress, uint16_t* Data);
static uint16_t EE_WriteVariable(uint16_t VirtAddress, uint16_t Data);

bool EE_ReadUShort(uint16_t VirtAddress, uint16_t* Data)
{
  return (EE_ReadVariable(VirtAddress, Data) == 0);
}

bool EE_WriteUShort(uint16_t VirtAddress, uint16_t Data)
{
  return (EE_WriteVariable(VirtAddress, Data) == FLASH_COMPLETE);
}

bool EE_ReadShort(uint16_t VirtAddress, int16_t* Data)
{
  return (EE_ReadVariable(VirtAddress, (uint16_t*) Data) == 0);
}

bool EE_WriteShort(uint16_t VirtAddress, int16_t Data)
{
  return (EE_WriteVariable(VirtAddress, (uint16_t) Data) == FLASH_COMPLETE);
}

bool EE_ReadUChar(uint16_t VirtAddress, uint8_t* Data)
{
  uint16_t temp;
  bool status;
  
  status = (EE_ReadVariable(VirtAddress, &temp) == 0);

  *Data = temp & 0xff;
  
  return status;
}

bool EE_WriteUChar(uint16_t VirtAddress, uint8_t Data)
{
  return (EE_WriteVariable(VirtAddress, (uint16_t) Data) == FLASH_COMPLETE);
}

bool EE_ReadChar(uint16_t VirtAddress, int8_t* Data)
{
  uint16_t temp;
  bool status;
  
  status = (EE_ReadVariable(VirtAddress, &temp) == 0);

  *Data = (int8_t) (temp & 0xff);
  
  return status;
}

bool EE_WriteChar(uint16_t VirtAddress, int8_t Data)
{
  return (EE_WriteVariable(VirtAddress, (uint16_t) Data) == FLASH_COMPLETE);
}

bool EE_ReadULong(uint16_t VirtAddress, uint32_t* Data)
{
  uint16_t temp_l, temp_h;
  bool status;

  status = (EE_ReadVariable(VirtAddress++, &temp_l) == 0);
  
  if (status != TRUE) {
    return status;
  } /* if (status != TRUE) */

  status = (EE_ReadVariable(VirtAddress, &temp_h) == 0);
  
  if (status != TRUE) {
    return status;
  } /* if (status != TRUE) */

  *Data = temp_h;
  *Data <<= 16;
  *Data |= temp_l;

  return status;
}

bool EE_WriteULong(uint16_t VirtAddress, uint32_t Data)
{
  uint16_t temp_l, temp_h;
  bool status;

  temp_l = Data & 0x0000ffff;
  Data >>= 16;
  temp_h = Data & 0x0000ffff;

  status = (EE_WriteVariable(VirtAddress++, temp_l) == FLASH_COMPLETE);

  if (status != TRUE) {
    return status;
  } /* if (status != TRUE) */
  
  return (EE_WriteVariable(VirtAddress, temp_h) == FLASH_COMPLETE);
}

bool EE_ReadLong(uint16_t VirtAddress, int32_t* Data)
{
  uint32_t temp;
  bool status;

  status = EE_ReadULong(VirtAddress, &temp);

  *Data = (int32_t) temp;

  return status;
}

bool EE_WriteLong(uint16_t VirtAddress, int32_t Data)
{
  return EE_WriteULong(VirtAddress, (uint32_t) Data);
}

bool EE_ReadString(uint16_t VirtAddress, char *Data, uint8_t *len)
{
  uint8_t l;
  uint16_t temp;
  bool status;

  status = EE_ReadChar(VirtAddress++, (int8_t *) &l);

  if (status != TRUE) {
    return status;
  } /* if (status != TRUE) */
  
  *len = l;

  while(l) {
    status = EE_ReadUShort(VirtAddress++, &temp);

    if (status != TRUE) {
      return status;
    } /* if (status != TRUE) */

    *Data = temp & 0xff;
    l--;
    Data++;

    if (l != 0) {
      temp >>= 8;
      *Data = temp & 0xff;
      l--;
      Data++;
    } /* if (l != 0) */

  } /* while(l) */

  return status;
}

bool EE_WriteString(uint16_t VirtAddress, char *Data, uint8_t len)
{
  uint8_t l;
  uint16_t temp, ad;
  bool status;

  ad = VirtAddress + 1;
  l = len;

  while(l) {

    if (l > 1) {
      temp = Data[1];
      temp <<= 8;
      temp |= Data[0];
      l-=2;
      Data += 2;
    } else { /* if (l > 1) */
      temp = Data[0];
      l--;
    } /* if (l > 1) */

    status = EE_WriteUShort(ad++, temp);

    if (status != TRUE) {
      return status;
    } /* if (status != TRUE) */

  } /* while(l) */

  return EE_WriteChar(VirtAddress, len);
}

/*
 * Restore the pages to a known good state in case of pages'
 *   status corruption after a power loss.
 * param None.
 * retval : - Flash error code: on write Flash error
 *          - FLASH_COMPLETE: on success
 */
uint16_t EE_Init(void)
{
  uint16_t PageStatus0 = 6, PageStatus1 = 6;
  uint16_t VarIdx = 0;
  uint16_t EepromStatus = 0, ReadStatus = 0;
  int16_t x = -1;
  uint16_t  FlashStatus;

  FLASH_Unlock();

  /* Get Page0 status */
  PageStatus0 = (*(__IO uint16_t*)PAGE0_BASE_ADDRESS);
  /* Get Page1 status */
  PageStatus1 = (*(__IO uint16_t*)PAGE1_BASE_ADDRESS);

  /* Check for invalid header states and repair if necessary */
  switch (PageStatus0) {
  case ERASED:

    /* Page0 erased, Page1 valid */
    if (PageStatus1 == VALID_PAGE) {

      /* Erase Page0 */
      FlashStatus = FLASH_ErasePage(PAGE0_BASE_ADDRESS);
      /* If erase operation was failed, a Flash error code is returned */
      if (FlashStatus != FLASH_COMPLETE) {
	goto ee_init_exit;
      } /* if (FlashStatus != FLASH_COMPLETE) */

    } else { /* if (PageStatus1 == VALID_PAGE) */

      /* Page0 erased, Page1 receive */
      if (PageStatus1 == RECEIVE_DATA) {
        /* Erase Page0 */
        FlashStatus = FLASH_ErasePage(PAGE0_BASE_ADDRESS);
        /* If erase operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE) {
	  goto ee_init_exit;
        } /* if (FlashStatus != FLASH_COMPLETE) */
        /* Mark Page1 as valid */
        FlashStatus = FLASH_ProgramHalfWord(PAGE1_BASE_ADDRESS, VALID_PAGE);
        /* If program operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE) {
	  goto ee_init_exit;
        } /*  if (FlashStatus != FLASH_COMPLETE) */
      } else { /* if (PageStatus1 == RECEIVE_DATA) */
	/* 
	 * First EEPROM access (Page0&1 are erased) or 
	 * invalid state -> format EEPROM 
	 */

        /* Erase both Page0 and Page1 and set Page0 as valid page */
        FlashStatus = EE_Format();

        /* 
	 * If erase/program operation was failed, 
	 * a Flash error code is returned 
	 */
        if (FlashStatus != FLASH_COMPLETE) {
	  goto ee_init_exit;
        } /* if (FlashStatus != FLASH_COMPLETE) */
      } /* if (PageStatus1 == RECEIVE_DATA) */
    } /* if (PageStatus1 == VALID_PAGE) */
    break;
    
  case RECEIVE_DATA:

    /* Page0 receive, Page1 valid */
    if (PageStatus1 == VALID_PAGE) {
      
      /* Transfer data from Page1 to Page0 */
      for (VarIdx = 0; VarIdx < NumbOfVar; VarIdx++) {
	if (( *(__IO uint16_t*)(PAGE0_BASE_ADDRESS + 6)) == 
	    VirtAddVarTab[VarIdx]) {
	  x = VarIdx;
	} /* if (( *(__IO uint16_t*)(PAGE0_BASE_ADDRESS + 6)) == ... */

	if (VarIdx != x) {
	  /* Read the last variables' updates */
	  ReadStatus = EE_ReadVariable(VirtAddVarTab[VarIdx], &DataVar);
	  /* In case variable corresponding to the virtual address was found */
	  if (ReadStatus != 0x1) {
	    /* Transfer the variable to the Page0 */
	    EepromStatus = 
	      EE_VerifyPageFullWriteVariable(VirtAddVarTab[VarIdx], DataVar);

	    /* 
	     * If program operation was failed, a Flash error code is returned 
	     */
	    if (EepromStatus != FLASH_COMPLETE) {
	       FlashStatus = EepromStatus;
	      goto ee_init_exit;
	    } /* if (EepromStatus != FLASH_COMPLETE) */
	  } /* if (ReadStatus != 0x1) */
	} /* if (VarIdx != x) */
      } /* for (VarIdx = 0; VarIdx < NumbOfVar; VarIdx++) */
      
      /* Mark Page0 as valid */
      FlashStatus = FLASH_ProgramHalfWord(PAGE0_BASE_ADDRESS, VALID_PAGE);

      /* If program operation was failed, a Flash error code is returned */
      if (FlashStatus != FLASH_COMPLETE) {
	goto ee_init_exit;
      } /* if (FlashStatus != FLASH_COMPLETE) */

      /* Erase Page1 */
      FlashStatus = FLASH_ErasePage(PAGE1_BASE_ADDRESS);

      /* If erase operation was failed, a Flash error code is returned */
      if (FlashStatus != FLASH_COMPLETE) {
	goto ee_init_exit;
      } /* if (FlashStatus != FLASH_COMPLETE) */

    } else { /* if (PageStatus1 == VALID_PAGE) */

      /* Page0 receive, Page1 erased */
      if (PageStatus1 == ERASED) {

        /* Erase Page1 */
        FlashStatus = FLASH_ErasePage(PAGE1_BASE_ADDRESS);

        /* If erase operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE) {
	  goto ee_init_exit;
        } /* if (FlashStatus != FLASH_COMPLETE) */

        /* Mark Page0 as valid */
        FlashStatus = FLASH_ProgramHalfWord(PAGE0_BASE_ADDRESS, VALID_PAGE);

        /* If program operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE) {
	  goto ee_init_exit;
        } /* if (FlashStatus != FLASH_COMPLETE) */
      
      } else { /* if (PageStatus1 == ERASED) */

	/* Invalid state -> format eeprom */
	
        /* Erase both Page0 and Page1 and set Page0 as valid page */
        FlashStatus = EE_Format();

        /* 
	 * If erase/program operation was failed, 
	 *  a Flash error code is returned 
	 */
        if (FlashStatus != FLASH_COMPLETE) {
	  goto ee_init_exit;
        } /* if (FlashStatus != FLASH_COMPLETE) */
      } /* if (PageStatus1 == ERASED) */
    } /* if (PageStatus1 == VALID_PAGE) */
    break;

  case VALID_PAGE:

    /* Invalid state -> format eeprom */
    if (PageStatus1 == VALID_PAGE) {

      /* Erase both Page0 and Page1 and set Page0 as valid page */
      FlashStatus = EE_Format();

      /*  
       * If erase/program operation was failed, 
       * a Flash error code is returned 
       */
      if (FlashStatus != FLASH_COMPLETE) {
	goto ee_init_exit;
      } /* if (FlashStatus != FLASH_COMPLETE) */
    } else { /* if (PageStatus1 == VALID_PAGE) */

      /* Page0 valid, Page1 erased */
      if (PageStatus1 == ERASED) {

        /* Erase Page1 */
        FlashStatus = FLASH_ErasePage(PAGE1_BASE_ADDRESS);

        /* If erase operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE) {
	goto ee_init_exit;
        } /* if (FlashStatus != FLASH_COMPLETE) */

      } else { /* if (PageStatus1 == ERASED) */

	/* Page0 valid, Page1 receive */
      
        /* Transfer data from Page0 to Page1 */
        for (VarIdx = 0; VarIdx < NumbOfVar; VarIdx++) {
          if ((*(__IO uint16_t*)(PAGE1_BASE_ADDRESS + 6)) == 
	      VirtAddVarTab[VarIdx]) {
            x = VarIdx;
          } /* if ((*(__IO uint16_t*)(PAGE1_BASE_ADDRESS + 6)) == ... */

          if (VarIdx != x) {
            /* Read the last variables' updates */
            ReadStatus = EE_ReadVariable(VirtAddVarTab[VarIdx], &DataVar);

            /* 
	     * In case variable corresponding to the virtual address was found 
	     */
            if (ReadStatus != 0x1) {
              /* Transfer the variable to the Page1 */
              EepromStatus = 
		EE_VerifyPageFullWriteVariable(VirtAddVarTab[VarIdx], DataVar);

              /* 
	       * If program operation was failed, 
	       * a Flash error code is returned 
	       */
              if (EepromStatus != FLASH_COMPLETE) {
		FlashStatus = EepromStatus;
		goto ee_init_exit;
              } /* if (EepromStatus != FLASH_COMPLETE) */
            } /* if (ReadStatus != 0x1) */
          } /* if (VarIdx != x) */
        } /* for (VarIdx = 0; VarIdx < NumbOfVar; VarIdx++) */

	/* Mark Page1 as valid */
        FlashStatus = FLASH_ProgramHalfWord(PAGE1_BASE_ADDRESS, VALID_PAGE);

        /* If program operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE) {
	  goto ee_init_exit;
        } /* if (FlashStatus != FLASH_COMPLETE) */

        /* Erase Page0 */
        FlashStatus = FLASH_ErasePage(PAGE0_BASE_ADDRESS);
        
	/* If erase operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE) {
	  goto ee_init_exit;
        } /* if (FlashStatus != FLASH_COMPLETE) */
      } /* if (PageStatus1 == ERASED) */
    } /* if (PageStatus1 == VALID_PAGE) */
    break;

    default:  
      /* Any other state -> format eeprom */
      
      /* Erase both Page0 and Page1 and set Page0 as valid page */
      FlashStatus = EE_Format();
      /* 
       * If erase/program operation was failed, 
       * a Flash error code is returned 
       */
      if (FlashStatus != FLASH_COMPLETE) {
	goto ee_init_exit;
      } /* if (FlashStatus != FLASH_COMPLETE) */
      break;

  } /* switch (PageStatus0) */

 ee_init_exit:
  
  printf("Test eeprom: %s\n", (FlashStatus == FLASH_COMPLETE) ? "Ok" : "Fail");

  return FlashStatus;
}

/*
 * Returns the last stored variable data, if found, which
 *   correspond to the passed virtual address
 * param VirtAddress: Variable virtual address
 * param Data: Global variable contains the read variable value
 * retval : - Success or error status:
 *      - 0: if variable was found
 *      - 1: if the variable was not found
 *      - NO_VALID_PAGE: if no valid page was found.
 */
static uint16_t EE_ReadVariable(uint16_t VirtAddress, uint16_t* Data)
{
  uint16_t ValidPage = PAGE0;
  uint16_t AddressValue = 0x5555, ReadStatus = 1;
  uint32_t Address, PageStartAddress;
  
  /* Get active Page for read operation */
  ValidPage = EE_FindValidPage(READ_FROM_VALID_PAGE);

  /* Check if there is no valid page */
  if (ValidPage == NO_VALID_PAGE) {
    return  NO_VALID_PAGE;
  } /* if (ValidPage == NO_VALID_PAGE) */

  /* Get the valid Page start Address */
  PageStartAddress = (uint32_t)(EEPROM_START_ADDRESS + 
				(uint32_t)(ValidPage * PAGE_SIZE));
  
  /* Get the valid Page end Address */
  Address = (uint32_t)((EEPROM_START_ADDRESS - 2) + 
		       (uint32_t)((1 + ValidPage) * PAGE_SIZE));
  
  /* Check each active page address starting from end */
  while (Address > (PageStartAddress + 2)) {

    /* Get the current location content to be compared with virtual address */
    AddressValue = (*(__IO uint16_t*)Address);

    /* Compare the read address with the virtual address */
    if (AddressValue == VirtAddress) {

      /* Get content of Address-2 which is variable value */
      *Data = (*(__IO uint16_t*)(Address - 2));

      /* In case variable value is read, reset ReadStatus flag */
      ReadStatus = 0;

      break;
    } else { /* if (AddressValue == VirtAddress) */
      /* Next address location */
      Address = Address - 4;
    } /* if (AddressValue == VirtAddress) */
  } /* while (Address > (PageStartAddress + 2)) */
  
  /* Return ReadStatus value: (0: variable exist, 1: variable doesn't exist) */
  return ReadStatus;
}

/*
 * Writes/upadtes variable data in EEPROM.
 * param VirtAddress: Variable virtual address
 * param Data: 16 bit data to be written
 * retval : - Success or error status:
 * - FLASH_COMPLETE: on success,
 * - PAGE_FULL: if valid page is full
 * - NO_VALID_PAGE: if no valid page was found
 * - Flash error code: on write Flash error
 */
static uint16_t EE_WriteVariable(uint16_t VirtAddress, uint16_t Data)
{
  uint16_t Status = 0;
  
  /* Write the variable virtual address and value in the EEPROM */
  Status = EE_VerifyPageFullWriteVariable(VirtAddress, Data);

  /* In case the EEPROM active page is full */
  if (Status == PAGE_FULL) {
    /* Perform Page transfer */
    Status = EE_PageTransfer(VirtAddress, Data);
  } /* if (Status == PAGE_FULL) */
  
  /* Return last operation status */
  return Status;
}

/*
 * Erases PAGE0 and PAGE1 and writes VALID_PAGE header to PAGE0
 * param  None
 * retval : Status of the last operation (Flash write or erase) done during
 *   EEPROM formating
 */
static FLASH_Status EE_Format(void)
{
  FLASH_Status FlashStatus = FLASH_COMPLETE;
  
  /* Erase Page0 */
  FlashStatus = FLASH_ErasePage(PAGE0_BASE_ADDRESS);

  /* If erase operation was failed, a Flash error code is returned */
  if (FlashStatus != FLASH_COMPLETE) {
    return FlashStatus;
  } /* if (FlashStatus != FLASH_COMPLETE) */
  
  /* Set Page0 as valid page: Write VALID_PAGE at Page0 base address */
  FlashStatus = FLASH_ProgramHalfWord(PAGE0_BASE_ADDRESS, VALID_PAGE);
  
  /* If program operation was failed, a Flash error code is returned */
  if (FlashStatus != FLASH_COMPLETE) {
    return FlashStatus;
  } /* if (FlashStatus != FLASH_COMPLETE) */

  /* Erase Page1 */
  FlashStatus = FLASH_ErasePage(PAGE1_BASE_ADDRESS);
  
  /* Return Page1 erase operation status */
  return FlashStatus;
}

/*
 * Find valid Page for write or read operation
 * param Operation: operation to achieve on the valid page:
 * arg READ_FROM_VALID_PAGE: read operation from valid page
 * arg WRITE_IN_VALID_PAGE: write operation from valid page
 * retval : Valid page number (PAGE0 or PAGE1) or NO_VALID_PAGE in case
 *   of no valid page was found
 */
static uint16_t EE_FindValidPage(uint8_t Operation)
{
  uint16_t PageStatus0 = 6, PageStatus1 = 6;
  
  /* Get Page0 actual status */
  PageStatus0 = (*(__IO uint16_t*)PAGE0_BASE_ADDRESS);
  
  /* Get Page1 actual status */
  PageStatus1 = (*(__IO uint16_t*)PAGE1_BASE_ADDRESS);
  
  /* Write or read operation */
  switch (Operation) {
  case WRITE_IN_VALID_PAGE:   
    /* ---- Write operation ---- */
    if (PageStatus1 == VALID_PAGE) {
      /* Page0 receiving data */
      if (PageStatus0 == RECEIVE_DATA) {
	return PAGE0;         /* Page0 valid */
      } else { /* if (PageStatus0 == RECEIVE_DATA) */
	return PAGE1;         /* Page1 valid */
      } /* if (PageStatus0 == RECEIVE_DATA) */
    } else { /* if (PageStatus1 == VALID_PAGE) */
      if (PageStatus0 == VALID_PAGE) {
        /* Page1 receiving data */
        if (PageStatus1 == RECEIVE_DATA) {
          return PAGE1;         /* Page1 valid */
        } else { /* if (PageStatus1 == RECEIVE_DATA) */
          return PAGE0;         /* Page0 valid */
        } /* if (PageStatus1 == RECEIVE_DATA) */
      } else { /* if (PageStatus0 == VALID_PAGE) */
        return NO_VALID_PAGE;   /* No valid Page */
      } /* if (PageStatus0 == VALID_PAGE) */
    } /* if (PageStatus1 == VALID_PAGE) */
    break;

  case READ_FROM_VALID_PAGE:  
    /* ---- Read operation ---- */
    if (PageStatus0 == VALID_PAGE) {
      return PAGE0;           /* Page0 valid */
    } else { /* if (PageStatus0 == VALID_PAGE) */
      if (PageStatus1 == VALID_PAGE) {
        return PAGE1;           /* Page1 valid */
      } else { /* if (PageStatus1 == VALID_PAGE) */
        return NO_VALID_PAGE ;  /* No valid Page */
      } /* if (PageStatus1 == VALID_PAGE) */
    } /* if (PageStatus0 == VALID_PAGE) */
    break;

  default:
    return PAGE0;             /* Page0 valid */
    break;
  } /* switch (Operation) */
}

/*
 * Verify if active page is full and Writes variable in EEPROM.
 * param VirtAddress: 16 bit virtual address of the variable
 * param Data: 16 bit data to be written as variable value
 * retval : - Success or error status:
 * - FLASH_COMPLETE: on success
 * - PAGE_FULL: if valid page is full
 * - NO_VALID_PAGE: if no valid page was found
 * - Flash error code: on write Flash error
 */
static uint16_t EE_VerifyPageFullWriteVariable(uint16_t VirtAddress, 
					       uint16_t Data)
{
  FLASH_Status FlashStatus = FLASH_COMPLETE;
  uint16_t ValidPage = PAGE0;
  uint32_t Address, PageEndAddress;

  /* Get valid Page for write operation */
  ValidPage = EE_FindValidPage(WRITE_IN_VALID_PAGE);

  /* Check if there is no valid page */
  if (ValidPage == NO_VALID_PAGE) {
    return  NO_VALID_PAGE;
  } /* if (ValidPage == NO_VALID_PAGE) */

  /* Get the valid Page start Address */
  Address = (uint32_t)(EEPROM_START_ADDRESS + 
		       (uint32_t)(ValidPage * PAGE_SIZE));
  
  /* Get the valid Page end Address */
  PageEndAddress = (uint32_t)((EEPROM_START_ADDRESS - 2) + 
			      (uint32_t)((1 + ValidPage) * PAGE_SIZE));
  
  /* Check each active page address starting from begining */
  while (Address < PageEndAddress) {

    /* Verify if Address and Address+2 contents are 0xFFFFFFFF */
    if ((*(__IO uint32_t*)Address) == 0xFFFFFFFF) {

      /* Set variable data */
      FlashStatus = FLASH_ProgramHalfWord(Address, Data);

      /* If program operation was failed, a Flash error code is returned */
      if (FlashStatus != FLASH_COMPLETE) {
        return FlashStatus;
      } /* if (FlashStatus != FLASH_COMPLETE) */

      /* Set variable virtual address */
      FlashStatus = FLASH_ProgramHalfWord(Address + 2, VirtAddress);
      /* Return program operation status */
      return FlashStatus;

    } else { /* if ((*(__IO uint32_t*)Address) == 0xFFFFFFFF) */
      /* Next address location */
      Address = Address + 4;
    } /* if ((*(__IO uint32_t*)Address) == 0xFFFFFFFF) */
  } /* while (Address < PageEndAddress) */
  
  /* Return PAGE_FULL in case the valid page is full */
  return PAGE_FULL;
}

/*
 * Transfers last updated variables data from the full Page to
 *   an empty one.
 * param VirtAddress: 16 bit virtual address of the variable
 * param Data: 16 bit data to be written as variable value
 * retval : - Success or error status:
 * - FLASH_COMPLETE: on success,
 * - PAGE_FULL: if valid page is full
 * - NO_VALID_PAGE: if no valid page was found
 * - Flash error code: on write Flash error
 */
static uint16_t EE_PageTransfer(uint16_t VirtAddress, uint16_t Data)
{
  FLASH_Status FlashStatus = FLASH_COMPLETE;
  uint32_t NewPageAddress, OldPageAddress;
  uint16_t ValidPage = PAGE0, VarIdx = 0;
  uint16_t EepromStatus = 0, ReadStatus = 0;

  /* Get active Page for read operation */
  ValidPage = EE_FindValidPage(READ_FROM_VALID_PAGE);

  if (ValidPage == PAGE1) {
    /* Page1 valid */

    /* New page address where variable will be moved to */
    NewPageAddress = PAGE0_BASE_ADDRESS;

    /* Old page address where variable will be taken from */
    OldPageAddress = PAGE1_BASE_ADDRESS;

  } else { /* if (ValidPage == PAGE1) */

    if (ValidPage == PAGE0)  {
      /* Page0 valid */

      /* New page address where variable will be moved to */
      NewPageAddress = PAGE1_BASE_ADDRESS;
      
      /* Old page address where variable will be taken from */
      OldPageAddress = PAGE0_BASE_ADDRESS;
    } else { /* if (ValidPage == PAGE0) */
      return NO_VALID_PAGE;       /* No valid Page */
    } /* if (ValidPage == PAGE0) */

  } /* if (ValidPage == PAGE1) */

  /* Set the new Page status to RECEIVE_DATA status */
  FlashStatus = FLASH_ProgramHalfWord(NewPageAddress, RECEIVE_DATA);

  /* If program operation was failed, a Flash error code is returned */
  if (FlashStatus != FLASH_COMPLETE) {
    return FlashStatus;
  } /* if (FlashStatus != FLASH_COMPLETE) */
  
  /* Write the variable passed as parameter in the new active page */
  EepromStatus = EE_VerifyPageFullWriteVariable(VirtAddress, Data);

  /* If program operation was failed, a Flash error code is returned */
  if (EepromStatus != FLASH_COMPLETE) {
    return EepromStatus;
  } /* if (EepromStatus != FLASH_COMPLETE) */

  /* Transfer process: transfer variables from old to the new active page */
  for (VarIdx = 0; VarIdx < NumbOfVar; VarIdx++) {
    
    /* Check each variable except the one passed as parameter */
    if (VirtAddVarTab[VarIdx] != VirtAddress) {

      /* Read the other last variable updates */
      ReadStatus = EE_ReadVariable(VirtAddVarTab[VarIdx], &DataVar);

      /* In case variable corresponding to the virtual address was found */
      if (ReadStatus != 0x1) {
        /* Transfer the variable to the new active page */
        EepromStatus = EE_VerifyPageFullWriteVariable(VirtAddVarTab[VarIdx], 
						      DataVar);

        /* If program operation was failed, a Flash error code is returned */
        if (EepromStatus != FLASH_COMPLETE) {
          return EepromStatus; 
        } /* if (EepromStatus != FLASH_COMPLETE) */
      } /* if (ReadStatus != 0x1) */
    } /* if (VirtAddVarTab[VarIdx] != VirtAddress) */
  } /* for (VarIdx = 0; VarIdx < NumbOfVar; VarIdx++) */

  /* Erase the old Page: Set old Page status to ERASED status */
  FlashStatus = FLASH_ErasePage(OldPageAddress);

  /* If erase operation was failed, a Flash error code is returned */
  if (FlashStatus != FLASH_COMPLETE) {
    return FlashStatus;
  } /* if (FlashStatus != FLASH_COMPLETE) */

  /* Set new Page status to VALID_PAGE status */
  FlashStatus = FLASH_ProgramHalfWord(NewPageAddress, VALID_PAGE);

  /* If program operation was failed, a Flash error code is returned */
  if (FlashStatus != FLASH_COMPLETE) {
    return FlashStatus;
  } /* if (FlashStatus != FLASH_COMPLETE) */

  /* Return last operation flash status */
  return FlashStatus;
}
