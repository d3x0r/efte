// so that we can reuse these numbers for other modes that re-use the C
// indent functions.

enum highlightSyntaxModes {
    hsC_Normal      =0,
    hsC_Comment     =1,
    hsC_CommentL    =2,
    hsC_Keyword     =4,
    hsC_String1     =10,
    hsC_String2     ,
    hsC_String3     ,
    hsC_CPP         ,
    hsC_CPP_Comm    ,
    hsC_CPP_String1 ,
    hsC_CPP_String2 ,
    hsC_CPP_ABrace
};
