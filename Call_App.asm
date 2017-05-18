	.def _jumpToAppEntry
	.text
_jumpToAppEntry:
	SETC INTM;
	ZAPA;
	MOV @SP,#0;
	PUSH ACC;
	PUSH AL;
	MOV AL, #0x0a08;
	PUSH AL;
	MOVL XAR7, #0x00307FF6;
	PUSH XAR7;
	POP RPC;
	POP ST1;
	POP ST0;
	POP IER;
	POP DBGIER;
	LRETR;
