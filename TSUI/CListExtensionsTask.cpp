//	ListExtensionsTask.cpp
//
//	ListExtensionsTask class
//	Copyright (c) 2013 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

#define ALIGN_CENTER							CONSTLIT("center")
#define ALIGN_RIGHT								CONSTLIT("right")

#define FILL_TYPE_NONE							CONSTLIT("none")

#define LINE_TYPE_SOLID							CONSTLIT("solid")

#define PROP_COLOR								CONSTLIT("color")
#define PROP_ENABLED							CONSTLIT("enabled")
#define PROP_FADE_EDGE_HEIGHT					CONSTLIT("fadeEdgeHeight")
#define PROP_FILL_TYPE							CONSTLIT("fillType")
#define PROP_FONT								CONSTLIT("font")
#define PROP_LINE_COLOR							CONSTLIT("lineColor")
#define PROP_LINE_PADDING						CONSTLIT("linePadding")
#define PROP_LINE_TYPE							CONSTLIT("lineType")
#define PROP_LINE_WIDTH							CONSTLIT("lineWidth")
#define PROP_LL_RADIUS							CONSTLIT("llRadius")
#define PROP_LR_RADIUS							CONSTLIT("lrRadius")
#define PROP_OPACITY							CONSTLIT("opacity")
#define PROP_PADDING_BOTTOM						CONSTLIT("paddingBottom")
#define PROP_POSITION							CONSTLIT("position")
#define PROP_SCALE								CONSTLIT("scale")
#define PROP_TEXT								CONSTLIT("text")
#define PROP_TEXT_ALIGN_HORZ					CONSTLIT("textAlignHorz")
#define PROP_UL_RADIUS							CONSTLIT("ulRadius")
#define PROP_UR_RADIUS							CONSTLIT("urRadius")

#define STYLE_SELECTION							CONSTLIT("selection")
#define STYLE_SELECTION_FOCUS					CONSTLIT("selectionFocus")

#define ERR_GET_COLLECTION_FAILED				CONSTLIT("Unable to get collection.")

const int ENTRY_ICON_HEIGHT =					150;
const int ENTRY_ICON_WIDTH =					300;

const int ICON_SPACING_HORZ =					16;
const int INTER_LINE_SPACING =					20;
const int MAJOR_PADDING_BOTTOM =				20;
const int MAJOR_PADDING_TOP =					20;
const int TYPE_CORNER_RADIUS =					3;
const int ICON_RADIUS =							6;

const int SELECTION_BORDER_WIDTH =				1;
const int SELECTION_CORNER_RADIUS =				8;

//	Right now, this class is a repurposed copy of CListCollectionTask

CListExtensionsTask::CListExtensionsTask(CHumanInterface &HI,
										  CExtensionCollection &Extensions,
										  int cxWidth,
										  bool bDebugMode) : IHITask(HI), 
		m_Extensions(Extensions),
		m_cxWidth(cxWidth),
		m_bDebugMode(bDebugMode),
		m_pList(NULL)

//	CListCollectionTask constructor

	{
	}

CListExtensionsTask::~CListExtensionsTask(void)

//	CListCollectionTask destructor

	{
	if (m_pList)
		delete m_pList;
	}

void CListExtensionsTask::CreateEntry (CExtension *pExtension, int yStart, IAnimatron **retpEntry, int *retcyHeight)

//	CreateEntry
//
//	Creates a new extension entry

	{
	const CVisualPalette &VI = m_HI.GetVisuals();
	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);
	const CG16bitFont &TypeFont = VI.GetFont(fontMediumHeavyBold);
	const CG16bitFont &SubTitleFont = VI.GetFont(fontSubTitle);

	int x = 0;
	int y = 0;
	int xText = x + ENTRY_ICON_WIDTH + ICON_SPACING_HORZ;
	int cxText = m_cxWidth - (ENTRY_ICON_WIDTH + ICON_SPACING_HORZ);

	//	Start with a sequencer

	CAniSequencer *pRoot = new CAniSequencer;
	pRoot->SetPropertyVector(PROP_POSITION, CVector(0, yStart));

	//	Note: We do not support embedded icons right now. Maybe later?
	/*
	//	Add the icon

	CG32bitImage *pIcon = 0;
	if (pIcon)
		{
		int xOffset = (ENTRY_ICON_WIDTH - pIcon->GetWidth()) / 2;
		IAnimatron *pIconAni = new CAniRoundedRect;
		pIconAni->SetPropertyVector(PROP_POSITION, CVector(x + xOffset, 0));
		pIconAni->SetPropertyVector(PROP_SCALE, CVector(pIcon->GetWidth(), pIcon->GetHeight()));
		pIconAni->SetPropertyInteger(PROP_UL_RADIUS, ICON_RADIUS);
		pIconAni->SetPropertyInteger(PROP_UR_RADIUS, ICON_RADIUS);
		pIconAni->SetPropertyInteger(PROP_LL_RADIUS, ICON_RADIUS);
		pIconAni->SetPropertyInteger(PROP_LR_RADIUS, ICON_RADIUS);
		pIconAni->SetFillMethod(new CAniImageFill(pIcon, true));

		pRoot->AddTrack(pIconAni, 0);
		}
	*/
	//	Add the extension name

	CString sHeading = pExtension->GetName();

	IAnimatron *pName = new CAniText;
	pName->SetPropertyVector(PROP_POSITION, CVector(xText, y));
	pName->SetPropertyVector(PROP_SCALE, CVector(10000, 1000));
	pName->SetPropertyColor(PROP_COLOR, VI.GetColor(colorTextDialogInput));
	pName->SetPropertyFont(PROP_FONT, &SubTitleFont);
	pName->SetPropertyString(PROP_TEXT, sHeading);

	pRoot->AddTrack(pName, 0);
	y += SubTitleFont.GetHeight();

	//	Add the type of extension

	CString sType;
	switch (pExtension->GetType())
		{
		case extAdventure:
			sType = CONSTLIT("Adventure");
			break;

		case extLibrary:
			sType = CONSTLIT("Library");
			break;

		default:
			sType = CONSTLIT("Expansion");
		}

	int cxTypeBackground = TypeFont.MeasureText(sType) + 2 * TYPE_CORNER_RADIUS;
	int cyTypeBackground = TypeFont.GetHeight() + 2 * TYPE_CORNER_RADIUS;

	IAnimatron *pTypeBackground = new CAniRoundedRect;
	pTypeBackground->SetPropertyColor(PROP_COLOR, VI.GetColor(colorTextNormal));
	pTypeBackground->SetPropertyOpacity(PROP_OPACITY, 255);
	pTypeBackground->SetPropertyInteger(PROP_UL_RADIUS, TYPE_CORNER_RADIUS);
	pTypeBackground->SetPropertyInteger(PROP_UR_RADIUS, TYPE_CORNER_RADIUS);
	pTypeBackground->SetPropertyInteger(PROP_LL_RADIUS, TYPE_CORNER_RADIUS);
	pTypeBackground->SetPropertyInteger(PROP_LR_RADIUS, TYPE_CORNER_RADIUS);
	pTypeBackground->SetPropertyVector(PROP_POSITION, CVector(xText, y + TYPE_CORNER_RADIUS));
	pTypeBackground->SetPropertyVector(PROP_SCALE, CVector(cxTypeBackground, TypeFont.GetHeight()));
	pRoot->AddTrack(pTypeBackground, 0);

	IAnimatron *pTypeLabel = new CAniText;
	pTypeLabel->SetPropertyVector(PROP_POSITION, CVector(xText + TYPE_CORNER_RADIUS, y + TYPE_CORNER_RADIUS));
	pTypeLabel->SetPropertyVector(PROP_SCALE, CVector(10000, 1000));
	pTypeLabel->SetPropertyColor(PROP_COLOR, VI.GetColor(colorAreaDeep));
	pTypeLabel->SetPropertyFont(PROP_FONT, &TypeFont);
	pTypeLabel->SetPropertyString(PROP_TEXT, sType);

	pRoot->AddTrack(pTypeLabel, 0);

	//	Add a version string

	CString sVersion;
	if (!pExtension->GetVersion().IsBlank())
		sVersion = strPatternSubst(CONSTLIT("version: %s"), pExtension->GetVersion());

	//	Add the version number

	if (!sVersion.IsBlank())
		{
		IAnimatron *pVersion = new CAniText;
		pVersion->SetPropertyVector(PROP_POSITION, CVector(xText + cxTypeBackground + ICON_SPACING_HORZ, y + TYPE_CORNER_RADIUS));
		pVersion->SetPropertyVector(PROP_SCALE, CVector(10000, 1000));
		pVersion->SetPropertyColor(PROP_COLOR, VI.GetColor(colorTextDialogLabel));
		pVersion->SetPropertyFont(PROP_FONT, &MediumFont);
		pVersion->SetPropertyString(PROP_TEXT, sVersion);

		pRoot->AddTrack(pVersion, 0);
		}

	y += cyTypeBackground;

	//	Add credits
	{
		CString sUNID = strPatternSubst("credits: %s", strJoin(pExtension->GetCredits(), CONSTLIT("oxfordComma")));

		IAnimatron *pUNID = new CAniText;
		pUNID->SetPropertyVector(PROP_POSITION, CVector(xText, y));
		pUNID->SetPropertyVector(PROP_SCALE, CVector(10000, 1000));
		pUNID->SetPropertyColor(PROP_COLOR, VI.GetColor(colorTextDialogLabel));
		pUNID->SetPropertyFont(PROP_FONT, &MediumFont);
		pUNID->SetPropertyString(PROP_TEXT, sUNID);

		pRoot->AddTrack(pUNID, 0);

		RECT rcLine;
		pUNID->GetSpacingRect(&rcLine);
		y += RectHeight(rcLine);
	}

	//	Add filespec
	{
		CString sFilespec = strPatternSubst("file: %s", pExtension->GetFilespec());

		IAnimatron *pFilespec = new CAniText;
		pFilespec->SetPropertyVector(PROP_POSITION, CVector(xText, y));
		pFilespec->SetPropertyVector(PROP_SCALE, CVector(10000, 1000));
		pFilespec->SetPropertyColor(PROP_COLOR, VI.GetColor(colorTextDialogLabel));
		pFilespec->SetPropertyFont(PROP_FONT, &MediumFont);
		pFilespec->SetPropertyString(PROP_TEXT, sFilespec);

		pRoot->AddTrack(pFilespec, 0);

		RECT rcLine;
		pFilespec->GetSpacingRect(&rcLine);
		y += RectHeight(rcLine);
	}

	//	Add UNID
		{
		CString sUNID = strPatternSubst("UNID: %x", pExtension->GetUNID());

		IAnimatron *pUNID = new CAniText;
		pUNID->SetPropertyVector(PROP_POSITION, CVector(xText, y));
		pUNID->SetPropertyVector(PROP_SCALE, CVector(10000, 1000));
		pUNID->SetPropertyColor(PROP_COLOR, VI.GetColor(colorTextDialogLabel));
		pUNID->SetPropertyFont(PROP_FONT, &MediumFont);
		pUNID->SetPropertyString(PROP_TEXT, sUNID);

		pRoot->AddTrack(pUNID, 0);

		RECT rcLine;
		pUNID->GetSpacingRect(&rcLine);
		y += RectHeight(rcLine);
		}

		//	Add disable-text
		if(pExtension->IsDisabled()) {
			CString sUNID = strPatternSubst("Disabled: %d", pExtension->GetDisabledReason());

			IAnimatron *pUNID = new CAniText;
			pUNID->SetPropertyVector(PROP_POSITION, CVector(xText, y));
			pUNID->SetPropertyVector(PROP_SCALE, CVector(10000, 1000));
			pUNID->SetPropertyColor(PROP_COLOR, VI.GetColor(colorTextDialogLabel));
			pUNID->SetPropertyFont(PROP_FONT, &MediumFont);
			pUNID->SetPropertyString(PROP_TEXT, sUNID);

			pRoot->AddTrack(pUNID, 0);

			RECT rcLine;
			pUNID->GetSpacingRect(&rcLine);
			y += RectHeight(rcLine);
		}

	//	Now add the description (this only works for Adventures, but maybe Extensions should support it too so that it shows up here
		{
		CString sDesc = pExtension->GetDesc();

		IAnimatron *pDesc = new CAniText;
		pDesc->SetPropertyVector(PROP_POSITION, CVector(xText, y));
		pDesc->SetPropertyVector(PROP_SCALE, CVector(cxText, 1000));
		pDesc->SetPropertyColor(PROP_COLOR, VI.GetColor(colorTextDialogInput));
		pDesc->SetPropertyFont(PROP_FONT, &MediumFont);
		pDesc->SetPropertyString(PROP_TEXT, sDesc);

		RECT rcLine;
		pDesc->GetSpacingRect(&rcLine);

		pRoot->AddTrack(pDesc, 0);
		y += RectHeight(rcLine);
		}
	//	Done

	*retpEntry = pRoot;

	if (retcyHeight)
		*retcyHeight = Max(ENTRY_ICON_HEIGHT, y);
	}

ALERROR CListExtensionsTask::OnExecute (ITaskProcessor *pProcessor, CString *retsResult)

//	OnExecute
//
//	Execute the task
	
	{
	int i;

	const CVisualPalette &VI = m_HI.GetVisuals();
	
	TArray<CExtension *> ExtensionList =  m_Extensions.GetExtensions();

	//	Sort the entries

	TSortMap<CString, CExtension *> SortedList;
	for (i = 0; i < ExtensionList.GetCount(); i++)
		{
		CExtension* pExtension = ExtensionList.GetAt(i);

		if (pExtension->IsRegistered())
			continue;

		//	Skip libraries unless in debug mode

		if (!m_bDebugMode && pExtension->GetType() == EExtensionTypes::extLibrary)
			continue;

		//	Sort key

		CString sSortKey = strPatternSubst(CONSTLIT("%s:%s"),
				(pExtension->GetType() == extAdventure ? CONSTLIT("01") : (pExtension->GetType() == extExtension ? CONSTLIT("02") : CONSTLIT("03"))),
				pExtension->GetName()
				);

		//	Add

		SortedList.Insert(sSortKey, pExtension);
		}

	//	Generate a Reanimator list of all entries. The root will be a CAniListBox

    VI.CreateListBox(NULL, NULL_STR, 0, 0, 0, 0, 0, &m_pList);

	//	No need to log image load

	g_pUniverse->SetLogImageLoad(false);

	//	Loop over all entries in the collection and add them to the list

	int y = MAJOR_PADDING_TOP;
	for (i = 0; i < SortedList.GetCount(); i++)
		{
		CExtension *pExtension = SortedList[i];

		//	Generate a record for the file

		IAnimatron *pEntry;
		int cyHeight;
		CreateEntry(pExtension, y, &pEntry, &cyHeight);

		m_pList->AddEntry(strFromInt(pExtension->GetUNID()), pEntry);
		y += cyHeight + INTER_LINE_SPACING;
		}

	g_pUniverse->SetLogImageLoad(true);

	//	Done

	return NOERROR;
	}
