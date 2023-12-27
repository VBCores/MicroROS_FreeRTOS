

extern FDCAN_TxHeaderTypeDef TxHeader;		// Header for can message
extern FDCAN_FilterTypeDef canfdfilterconfig;	// Filter for receiving CAN messages
extern FDCAN_RxHeaderTypeDef RxHeader;

//FDCAN_HandleTypeDef hfdcan1;


void FDCAN_Filter_Config(FDCAN_FilterTypeDef* sFilterConfig);

void FDCAN_Header_Config(FDCAN_TxHeaderTypeDef* TxHeader);

void FDCAN_Starter(FDCAN_HandleTypeDef* hfdcan, FDCAN_FilterTypeDef* sFilterConfig);
