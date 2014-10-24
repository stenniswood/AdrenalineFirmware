


// MUST INCLUDE LCD_ICONS.H BEFORE INCLUDING THIS FILE!
void DD_DrawAllOn			();
void DD_DrawAllOff			();
void DD_DrawZigZag			();

void DD_Inverse_Video		(byte mLine, byte mStart, byte mEnd 				);
void DD_SetPixel			(byte mVertical, byte mColumn						);
void DD_DrawHorizontalLine	(byte mVertical, byte mStart, byte mEnd				);
void DD_DrawVerticalLine	(byte mStart, byte mEnd, byte mColumn				);
void DD_DrawBox 			(byte mLinePixStart, byte mLinePixEnd, byte mColumnStart, byte mColumnEnd);
void DD_Display_YesNo_Box	(_far char* mText, byte mHighlight					);
void DD_Display_OkCancel_Box(_far char* mText, byte mHighlight					);
void DD_DrawIcon			(byte origx, byte origy, _far struct stIcon* mIcon 	);
void DD_EraseLine			(byte mLine, byte mStartCol, byte mEndCol			);
void DD_Clear_EOL			(													);


