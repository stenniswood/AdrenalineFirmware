struct stHorizontalSBPosition
{
	byte LCD_Line;
	byte LCD_StartColumn;
	byte LCD_EndColumn;
	byte LCD_CurrentPosition;	
	word Data_MaxPosition;
	word Data_MinPosition;
	word Data_CurrPosition;
};

typedef struct stHorizontalSBPosition stHSB_Data;

void SB_SetRange	( stHSB_Data* mHSB, word MaxPos, word MinPos);
void SB_SetPosition	(stHSB_Data* mHSB, word NewPos);
void SB_SetPlacementH(stHSB_Data* mHSB, byte mLine, byte mStartColumn, byte mEndColumn);
void Draw_HScrollBar(stHSB_Data* mHSB);





