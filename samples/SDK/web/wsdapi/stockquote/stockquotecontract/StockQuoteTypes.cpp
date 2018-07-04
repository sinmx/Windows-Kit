// <Include>
///////////////////////////////////////////////////////////////////////////////
//
// THIS FILE IS AUTOMATICALLY GENERATED.  DO NOT MODIFY IT BY HAND.
//
///////////////////////////////////////////////////////////////////////////////
// </Include>

// <LiteralInclude>
#include <wsdapi.h>
// </LiteralInclude>

// <LiteralInclude>
#include "StockQuote.h"
// </LiteralInclude>

// <LiteralInclude>
#include "StockQuoteTypes.h"
// </LiteralInclude>

// <NamespaceDefinitions>
//
// Namespace http://example.com/stockquote/definitions
//
WSDXML_NAME Names_Definitions[] =
{   { &Namespace_Definitions, L"StockQuotePortType" }
,   { &Namespace_Definitions, L"GetLastTradePriceInput" }
,   { &Namespace_Definitions, L"GetLastTradePriceOutput" }
,   { &Namespace_Definitions, L"GetLastTradePrice" }
,   { &Namespace_Definitions, L"body" }
,   { &Namespace_Definitions, L"TradePriceRequest" }
,   { &Namespace_Definitions, L"TradePrice" }
,   { &Namespace_Definitions, L"StockQuoteDeviceType" }
};
 WSDXML_NAMESPACE Namespace_Definitions =
{   L"http://example.com/stockquote/definitions"
,   L"def"
,   &Names_Definitions[0]
,   8
,   0
};

//
// Namespace http://example.com/stockquote/schemas
//
WSDXML_NAME Names_Schemas[] =
{   { &Namespace_Schemas, L"TradePriceRequest" }
,   { &Namespace_Schemas, L"TradePrice" }
,   { &Namespace_Schemas, L"tickerSymbol" }
,   { &Namespace_Schemas, L"price" }
};
 WSDXML_NAMESPACE Namespace_Schemas =
{   L"http://example.com/stockquote/schemas"
,   L"sch"
,   &Names_Schemas[0]
,   4
,   1
};

//
// Namespace http://www.w3.org/2001/XMLSchema
//
WSDXML_NAME Names_XMLSchema[] =
{   { &Namespace_XMLSchema, L"anyType" }
};
 WSDXML_NAMESPACE Namespace_XMLSchema =
{   L"http://www.w3.org/2001/XMLSchema"
,   L"xml"
,   &Names_XMLSchema[0]
,   1
,   2
};

WSDXML_NAMESPACE* StockQuoteNamespaces[] =
{   &Namespace_Definitions
,   &Namespace_Schemas
,   &Namespace_XMLSchema
};

// </NamespaceDefinitions>

// <TypeTableDefinitions>
static BYTE TypeTable_TRADE_PRICE_REQUEST[] =
{   OpFormatPointerToStruct(TRADE_PRICE_REQUEST)
,   OpBeginSequence
,       OpBeginElement(SCHEMAS_TickerSymbol)
,           OpFormatUnicodeString(TRADE_PRICE_REQUEST,tickerSymbol)
,       OpEndElement
,   OpEndSequence
,   OpEndOfTable
};
WSDXML_TYPE Type_TRADE_PRICE_REQUEST =
{   NULL
,   TypeTable_TRADE_PRICE_REQUEST
};

static BYTE TypeTable_TRADE_PRICE[] =
{   OpFormatPointerToStruct(TRADE_PRICE)
,   OpBeginSequence
,       OpBeginElement(SCHEMAS_Price)
,           OpFormatFloat(TRADE_PRICE,price,0)
,       OpEndElement
,   OpEndSequence
,   OpEndOfTable
};
WSDXML_TYPE Type_TRADE_PRICE =
{   NULL
,   TypeTable_TRADE_PRICE
};

WSDXML_TYPE* StockQuoteTypes[] =
{   &Type_TRADE_PRICE_REQUEST
,   &Type_TRADE_PRICE
};

// </TypeTableDefinitions>

// <MessageTypeDefinitions>
//
// Port type http://example.com/stockquote/definitions/StockQuotePortType
// Message type definitions
//
static BYTE RequestBodyType_GetLastTradePrice[] =
{   OpFormatPointerToStruct(REQUESTBODY_StockQuotePortType_GetLastTradePrice)
,   OpBeginElement(DEFINITIONS_TradePriceRequest)
,       OpFormatType(TYPE_ENCODING_TRADE_PRICE_REQUEST,REQUESTBODY_StockQuotePortType_GetLastTradePrice,body)
,   OpEndElement
,   OpEndOfTable
};

WSDXML_TYPE REQUESTTYPE_GetLastTradePrice =
{   L"http://example.com/stockquote/definitions/StockQuotePortType/GetLastTradePriceRequest"
,   RequestBodyType_GetLastTradePrice
};

static BYTE ResponseBodyType_GetLastTradePrice[] =
{   OpFormatPointerToStruct(RESPONSEBODY_StockQuotePortType_GetLastTradePrice)
,   OpBeginElement(DEFINITIONS_TradePrice)
,       OpFormatType(TYPE_ENCODING_TRADE_PRICE,RESPONSEBODY_StockQuotePortType_GetLastTradePrice,body)
,   OpEndElement
,   OpEndOfTable
};

WSDXML_TYPE RESPONSETYPE_GetLastTradePrice =
{   L"http://example.com/stockquote/definitions/StockQuotePortType/GetLastTradePriceResponse"
,   ResponseBodyType_GetLastTradePrice
};


//
// Port type http://example.com/stockquote/definitions/StockQuoteDeviceType
// Message type definitions
//

// </MessageTypeDefinitions>

// <PortTypeDefinitions>
//
// Port type http://example.com/stockquote/definitions/StockQuotePortType
//
WSD_OPERATION Operations_StockQuotePortType[] =
{   {   &REQUESTTYPE_GetLastTradePrice
    ,   &RESPONSETYPE_GetLastTradePrice
    ,   (WSD_STUB_FUNCTION)Stub_GetLastTradePrice
    }
};

WSD_PORT_TYPE PortType_StockQuotePortType =
{   DEFINITIONS_StockQuotePortType
,   (sizeof(Operations_StockQuotePortType) / sizeof(Operations_StockQuotePortType[0]))
,   Operations_StockQuotePortType
,   WSD_PT_ALL
};

// </PortTypeDefinitions>

// <RelationshipMetadataDefinition>
static WSD_NAME_LIST serviceTypes_0[] =
{   {   NULL
    ,   &Names_Definitions[0] 
    }
};

static WSD_SERVICE_METADATA services_0[] =
{   {   NULL
    ,   &serviceTypes_0[0]
    ,   L"http://example.com/stockquote/definitions/StockQuotePortType0"
    ,   NULL
    }
};

static WSD_NAME_LIST hostTypes_0[] =
{   {   NULL
    ,   &Names_Definitions[7] 
    }
};

WSD_SERVICE_METADATA hostServiceMetadata_0 =
{   NULL
,   &hostTypes_0[0]
,   L"http://example.com/stockquote/definitions/StockQuoteDevice"
,   NULL
};

static WSD_SERVICE_METADATA_LIST hostedServiceMetadataList_0[] =
{   {   NULL
    ,   &services_0[0]
    }
};

WSD_HOST_METADATA hostMetadata = 
{   
        &hostServiceMetadata_0
    ,   hostedServiceMetadataList_0
};
// </RelationshipMetadataDefinition>

// <ThisModelMetadataDefinition>
static WSD_LOCALIZED_STRING modelNames[] =
{   {   NULL
    ,   L"Microsoft StockQuote Device Emulator"
    }
};

static WSD_LOCALIZED_STRING_LIST modelNameList[] =
{   {   NULL
    ,   &modelNames[0]
    }
};

static WSD_LOCALIZED_STRING manufacturers[] =
{   {   NULL
    ,   L"Microsoft Corporation"
    }
};

static WSD_LOCALIZED_STRING_LIST manufacturerList[] =
{
    {   NULL
    ,   &manufacturers[0]
    },
};

WSD_THIS_MODEL_METADATA thisModelMetadata =
{   manufacturerList
,   L"http://www.microsoft.com/"
,   modelNameList
,   L"MSQDE-0001"
,   L"http://www.microsoft.com/"
,   NULL
,   NULL // any
};
// </ThisModelMetadataDefinition>

