//	CTradingDesc.cpp
//
//	CTradingDesc class

#include "PreComp.h"

#define ACCEPT_DONATION_TAG						CONSTLIT("AcceptDonation")
#define BUY_TAG									CONSTLIT("Buy")
#define INSTALL_ARMOR_TAG						CONSTLIT("InstallArmor")
#define INSTALL_DEVICE_TAG						CONSTLIT("InstallDevice")
#define REFUEL_TAG								CONSTLIT("Refuel")
#define REMOVE_DEVICE_TAG						CONSTLIT("RemoveDevice")
#define REPAIR_ARMOR_TAG						CONSTLIT("RepairArmor")
#define SELL_TAG								CONSTLIT("Sell")

#define ACTUAL_PRICE_ATTRIB						CONSTLIT("actualPrice")
#define CURRENCY_ATTRIB							CONSTLIT("currency")
#define CREDIT_CONVERSION_ATTRIB				CONSTLIT("creditConversion")
#define CRITERIA_ATTRIB							CONSTLIT("criteria")
#define INVENTORY_ADJ_ATTRIB					CONSTLIT("inventoryAdj")
#define ITEM_ATTRIB								CONSTLIT("item")
#define LEVEL_FREQUENCY_ATTRIB					CONSTLIT("levelFrequency")
#define MAX_ATTRIB								CONSTLIT("max")
#define MESSAGE_ID_ATTRIB						CONSTLIT("messageID")
#define NO_DESCRIPTION_ATTRIB					CONSTLIT("noDescription")
#define PRICE_ADJ_ATTRIB						CONSTLIT("priceAdj")
#define REPLENISH_ATTRIB						CONSTLIT("replenish")
#define UPGRADE_INSTALL_ONLY_ATTRIB				CONSTLIT("upgradeInstallOnly")

#define CONSTANT_PREFIX							CONSTLIT("constant")
#define UNAVAILABLE_PREFIX						CONSTLIT("unavailable")
#define VARIABLE_PREFIX							CONSTLIT("variable")

const int EXTRA_REPAIR_COST_FACTOR =            3;  //  Damage over 50% cost 3 times more to repair

struct SServiceData
	{
	char *pszName;
	char *pszIDPrefix;
	char *pszTag;
	};

static SServiceData SERVICE_DATA[serviceCount] =
	{
		{	"(serviceNone)",			"",		""				},	//	serviceNone

		{	"priceOfferedToPlayer",		"B",	"Buy"			},	//	serviceBuy
		{	"priceForPlayerToBuy",		"S",	"Sell"			},	//	serviceSell
		{	"valueOfDonation",			"A",	"AcceptDonation"},	//	serviceAcceptDonations
		{	"priceToRefuel",			"F",	"Refuel"		},	//	serviceRefuel
		{	"priceToRepairArmor",		"Ra",	"RepairArmor"	},	//	serviceRepairArmor

		{	"priceToReplaceArmor",		"Ia",	"ReplaceArmor"	},	//	serviceReplaceArmor
		{	"priceToInstallDevice",		"Id",	"InstallDevice"	},	//	serviceInstallDevice
		{	"priceToRemoveDevice",		"Vd",	"RemoveDevice"	},	//	serviceRemoveDevice
		{	"priceToUpgradeDevice",		"Ud",	"UpgradeDevice"	},	//	serviceUpgradeDevice
		{	"priceToEnhanceItem",		"Ei",	"EnhanceItem"	},	//	serviceEnhanceItem

		{	"priceToRepairItem",		"Ri",	"RepairItem"	},	//	serviceRepairItem
		{	"customPrice",				"X",	"Custom"		},	//	serviceCustom
		{	"priceOfShipOffered",		"Bs",	"BuyShip"		},	//	serviceBuyShip
		{	"priceOfShipForSale",		"Ss",	"SellShip"		},	//	serviceSellShip
	};

CTradingDesc::CTradingDesc (void) : 
		m_iMaxCurrency(0),
		m_iReplenishCurrency(0)

//	CTradingDesc constructor

	{
	}

CTradingDesc::~CTradingDesc (void)

//	CTradingDesc destructor

	{
	}

void CTradingDesc::AddOrder (CItemType *pItemType, const CString &sCriteria, int iPriceAdj, DWORD dwFlags)

//	AddOrder
//
//	Add a new commodity line

	{
	int i;

	//	Convert to new method

	ETradeServiceTypes iService;
	DWORD dwNewFlags;
	ReadServiceFromFlags(dwFlags, &iService, &dwNewFlags);
	if (iService == serviceNone)
		return;

	//	We always add at the beginning (because new orders take precedence)

	SServiceDesc *pCommodity = m_List.InsertAt(0);
	pCommodity->iService = iService;
	pCommodity->pItemType = pItemType;
	if (pItemType == NULL)
		CItem::ParseCriteria(sCriteria, &pCommodity->ItemCriteria);
	pCommodity->PriceAdj.SetInteger(iPriceAdj);
	pCommodity->dwFlags = dwNewFlags;
	pCommodity->sID = ComputeID(iService, pCommodity->pItemType.GetUNID(), sCriteria, dwNewFlags);

	//	If we find a previous order for the same criteria, then delete it

	for (i = 1; i < m_List.GetCount(); i++)
		if (strEquals(pCommodity->sID, m_List[i].sID))
			{
			m_List.Delete(i);
			break;
			}
	}

int CTradingDesc::CalcPriceForService (ETradeServiceTypes iService, CSpaceObject *pProvider, const CItem &Item, int iCount, DWORD dwFlags)

//	CalcPriceForService
//
//	Compute a price for a service (used by objects without a Trade descriptor).

	{
	SServiceDesc Default;
	Default.iService = iService;
	Default.PriceAdj.InitFromInteger(100);
	Default.dwFlags = 0;

	STradeServiceCtx Ctx;
	Ctx.iService = iService;
	Ctx.pProvider = pProvider;
	Ctx.pCurrency = pProvider->GetDefaultEconomy();
	Ctx.pItem = &Item;
	Ctx.iCount = iCount;

	return ComputePrice(Ctx, Default, dwFlags);
	}

bool CTradingDesc::ComposeDescription (CString *retsDesc) const

//  ComposeDescription
//
//  Composes a description of the trading desc, suitable for map description.
//  We return FALSE if we have no relevant services.

    {
    CString sDesc;

    //  Short-circuit

    if (!HasServices())
        return false;

    //  Buying and selling ships

    SServiceInfo BuyShips;
    GetServiceInfo(serviceBuyShip, BuyShips);

    SServiceInfo SellShips;
    GetServiceInfo(serviceSellShip, SellShips);

    CString sBuySellShips;
    if (BuyShips.bAvailable && SellShips.bAvailable)
        sDesc = CONSTLIT("Buys and sells ships");
    else if (BuyShips.bAvailable)
        sDesc = CONSTLIT("Buys ships");
    else if (BuyShips.bAvailable)
        sDesc = CONSTLIT("Sells ships");

    //  Refuels

    SServiceInfo Refuel;
    if (GetServiceInfo(serviceRefuel, Refuel))
        {
        CString sText = strPatternSubst(CONSTLIT("Refuels up to level %d"), Refuel.iMaxLevel);

        if (!sDesc.IsBlank())
            sDesc = strPatternSubst(CONSTLIT("%s � %s"), sDesc, sText);
        else
            sDesc = sText;
        }

    //  Repair armor

    SServiceInfo RepairArmor;
    GetServiceInfo(serviceRepairArmor, RepairArmor);

    SServiceInfo InstallArmor;
    GetServiceInfo(serviceReplaceArmor, InstallArmor);

    if (RepairArmor.iMaxLevel != -1 || InstallArmor.iMaxLevel != -1)
        {
        CString sPurchased = (InstallArmor.bUpdateInstallOnly ? CONSTLIT("purchased ") : NULL_STR);

        CString sText;
        if (RepairArmor.iMaxLevel == InstallArmor.iMaxLevel)
            sText = strPatternSubst(CONSTLIT("Repairs/installs %sarmor up to level %d"), sPurchased, RepairArmor.iMaxLevel);
        else if (RepairArmor.iMaxLevel != -1 && InstallArmor.iMaxLevel != -1)
            sText = strPatternSubst(CONSTLIT("Repairs armor up to level %d � Installs %sarmor up to level %d"), RepairArmor.iMaxLevel, sPurchased, InstallArmor.iMaxLevel);
        else if (RepairArmor.iMaxLevel != -1)
            sText = strPatternSubst(CONSTLIT("Repairs armor up to level %d"), RepairArmor.iMaxLevel);
        else
            sText = strPatternSubst(CONSTLIT("Installs %sarmor up to level %d"), sPurchased, InstallArmor.iMaxLevel);

        if (!sDesc.IsBlank())
            sDesc = strPatternSubst(CONSTLIT("%s � %s"), sDesc, sText);
        else
            sDesc = sText;
        }

    //  Install devices

    SServiceInfo InstallDevice;
    GetServiceInfo(serviceInstallDevice, InstallDevice);

    if (InstallDevice.iMaxLevel != -1)
        {
        CString sPurchased = (InstallDevice.bUpdateInstallOnly ? CONSTLIT("purchased ") : NULL_STR);
        CString sText = strPatternSubst(CONSTLIT("Installs %sdevices up to level %d"), sPurchased, InstallDevice.iMaxLevel);

        if (!sDesc.IsBlank())
            sDesc = strPatternSubst(CONSTLIT("%s � %s"), sDesc, sText);
        else
            sDesc = sText;
        }

    SServiceInfo Buys;
    GetServiceInfo(serviceBuy, Buys);

    SServiceInfo Sells;
    GetServiceInfo(serviceSell, Sells);

    if (Buys.bAvailable || Sells.bAvailable)
        {
        CString sText;
        if (Buys.bAvailable && Sells.bAvailable)
            sText = CONSTLIT("Buys and sells commodities");
        else if (Buys.bAvailable)
            sText = CONSTLIT("Buys commodities");
        else
            sText = CONSTLIT("Sells commodities");

        if (!sDesc.IsBlank())
            sDesc = strPatternSubst(CONSTLIT("%s � %s"), sDesc, sText);
        else
            sDesc = sText;
        }

    //  If we don't have any trade descriptor, then we're done

    if (sDesc.IsBlank())
        return false;

    //  Return it

    if (retsDesc)
        *retsDesc = sDesc;

    return true;
    }

CString CTradingDesc::ComputeID (ETradeServiceTypes iService, DWORD dwUNID, const CString &sCriteria, DWORD dwFlags)

//	ComputeID
//
//	Generates a string ID for the order. Two identical orders should have the same ID

	{
	CString sService = ((iService >= 0 && iService < serviceCount) ? CString(SERVICE_DATA[iService].pszIDPrefix) : CONSTLIT("?"));

	if (dwUNID)
		return strPatternSubst(CONSTLIT("%s:%x"), sService, dwUNID);
	else
		return strPatternSubst(CONSTLIT("%s:%s"), sService, sCriteria);
	}

int CTradingDesc::ComputeMaxCurrency (CSpaceObject *pObj)

//	ComputeMaxCurrency
//
//	Computes max balance

	{
	return m_iMaxCurrency * (90 + ((pObj->GetDestiny() + 9) / 18)) / 100;
	}

int CTradingDesc::ComputePrice (STradeServiceCtx &Ctx, DWORD dwFlags)

//	ComputePrice
//
//	Computes the price

	{
	//	For ships we match by object

	const SServiceDesc *pDesc;
	if (Ctx.iService == serviceBuyShip
			|| Ctx.iService == serviceSellShip)
		{
		ASSERT(Ctx.pObj);

		if (!FindService(Ctx.iService, Ctx.pObj, &pDesc))
			return -1;
		}

	//	Otherwise, match by item

	else
		{
		ASSERT(Ctx.pItem);

		if (!FindService(Ctx.iService, *Ctx.pItem, &pDesc))
			return -1;
		}

	//	Now we can compute the price

	return ComputePrice(Ctx, *pDesc, dwFlags);
	}

int CTradingDesc::ComputePrice (STradeServiceCtx &Ctx, const SServiceDesc &Commodity, DWORD dwFlags)

//	ComputePrice
//
//	Computes the price of the item in the proper currency

	{
	bool bActual = (Commodity.dwFlags & FLAG_ACTUAL_PRICE ? true : false);
	bool bPlayerAdj = !(dwFlags & FLAG_NO_PLAYER_ADJ);

	//	Get the raw price from the item

	int iBasePrice;
	CEconomyType *pBaseEconomy;
	switch (Commodity.iService)
		{
		case serviceRepairArmor:
			{
			CArmorClass *pArmor = Ctx.pItem->GetType()->GetArmorClass();
			if (pArmor == NULL)
				return -1;

            //  If we don't have a source, then we only do a basic calculation. 
            //  This should not happen (as long as the TLisp code follows 
            //  guidelines).

            if (Ctx.pObj == NULL)
    			iBasePrice = Ctx.iCount * pArmor->GetRepairCost(CItemCtx(*Ctx.pItem));

            //  Otherwise, the price is based on how much damage we've taken

            else
                {
                CInstalledArmor *pSeg = Ctx.pObj->FindArmor(*Ctx.pItem);
                if (pSeg == NULL)
                    return -1;

                CItemCtx ItemCtx(Ctx.pItem, Ctx.pObj, pSeg);

                int iMaxHP = pArmor->GetMaxHP(ItemCtx);
                int iHalf = iMaxHP / 2;

                //  We can repair up to half of maximum damage at normal price

                int iHPToRepair = Ctx.iCount;
                int iHPAtNormalPrice = Min(iHalf, iHPToRepair);
                iBasePrice = iHPAtNormalPrice * pArmor->GetRepairCost(ItemCtx);

                //  If we have more damage than that, we pay double price

                int iHPAtExtraPrice = iHPToRepair - iHPAtNormalPrice;
                iBasePrice += iHPAtExtraPrice * pArmor->GetRepairCost(ItemCtx) * EXTRA_REPAIR_COST_FACTOR;
                }

			pBaseEconomy = Ctx.pItem->GetCurrencyType();
			break;
			}

		case serviceInstallDevice:
		case serviceReplaceArmor:
			{
			iBasePrice = Ctx.iCount * Ctx.pItem->GetType()->GetInstallCost(CItemCtx(*Ctx.pItem));
			pBaseEconomy = Ctx.pItem->GetCurrencyType();
			break;
			}

		case serviceRemoveDevice:
			{
			iBasePrice = Ctx.iCount * Ctx.pItem->GetType()->GetInstallCost(CItemCtx(*Ctx.pItem)) / 2;
			pBaseEconomy = Ctx.pItem->GetCurrencyType();
			break;
			}

		case serviceBuyShip:
		case serviceSellShip:
			{
			CCurrencyAndValue Value = Ctx.pObj->GetTradePrice(Ctx.pProvider);
			iBasePrice = (int)Value.GetValue();
			pBaseEconomy = Value.GetCurrencyType();
			break;
			}

		case serviceBuy:
			{
			//	If this item cannot be sold when used, then check if it is used.

			if (!Ctx.pItem->GetType()->CanBeSoldIfUsed()
					&& Ctx.pItem->IsUsed())
				return -1;

			//	Otherwise, normal price

			iBasePrice = Ctx.iCount * Ctx.pItem->GetTradePrice(Ctx.pProvider, bActual);
			pBaseEconomy = Ctx.pItem->GetCurrencyType();
			break;
			}

		default:
			iBasePrice = Ctx.iCount * Ctx.pItem->GetTradePrice(Ctx.pProvider, bActual);
			pBaseEconomy = Ctx.pItem->GetCurrencyType();
			break;
		}

	//	Adjust the price appropriately

	bool bEventAdj = false;
	CurrencyValue iPrice;
	CString sPrefix;
	int iPriceAdj = Commodity.PriceAdj.EvalAsInteger(Ctx.pProvider, &sPrefix);

	if (sPrefix.IsBlank())
		iPrice = iPriceAdj * iBasePrice / 100;
	else if (strEquals(sPrefix, CONSTANT_PREFIX))
		iPrice = iPriceAdj;
	else if (strEquals(sPrefix, UNAVAILABLE_PREFIX))
		return -1;
	else if (strEquals(sPrefix, VARIABLE_PREFIX))
		{
		iPrice = iBasePrice;
		bEventAdj = true;
		}
	else
		{
		kernelDebugLogPattern("Unknown priceAdj prefix: %s", sPrefix);
		return -1;
		}

	//	Let the provide adjust prices, if it has an event

	int iPlayerPriceAdj;
	if (bPlayerAdj 
			&& bEventAdj
			&& Ctx.pProvider
			&& Ctx.pProvider->FireGetPlayerPriceAdj(Ctx, NULL, &iPlayerPriceAdj))
		{
		if (iPlayerPriceAdj < 0)
			return -1;

		iPrice = iPlayerPriceAdj * iPrice / 100;
		}

	//	Let global types adjust the price

	if (bPlayerAdj 
			&& g_pUniverse->GetDesignCollection().FireGetGlobalPlayerPriceAdj(Ctx, NULL, &iPlayerPriceAdj))
		{
		if (iPlayerPriceAdj < 0)
			return -1;

		iPrice = iPlayerPriceAdj * iPrice / 100;
		}

	//	Convert to proper currency

	return (int)Ctx.pCurrency->Exchange(pBaseEconomy, iPrice);
	}

ALERROR CTradingDesc::CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CTradingDesc **retpTrade)

//	InitFromXML
//
//	Initialize from an XML element

	{
	ALERROR error;
	int i, j;

	//	Allocate the trade structure

	CTradingDesc *pTrade = new CTradingDesc;
	pTrade->m_pCurrency.LoadUNID(pDesc->GetAttribute(CURRENCY_ATTRIB));
	pTrade->m_iMaxCurrency = pDesc->GetAttributeIntegerBounded(MAX_ATTRIB, 0, -1, 0);
	pTrade->m_iReplenishCurrency = pDesc->GetAttributeIntegerBounded(REPLENISH_ATTRIB, 0, -1, 0);

	//	Allocate the array

	int iCount = pDesc->GetContentElementCount();
	if (iCount)
		{
		pTrade->m_List.InsertEmpty(iCount);

		//	Load

		for (i = 0; i < iCount; i++)
			{
			CXMLElement *pLine = pDesc->GetContentElement(i);
			SServiceDesc *pCommodity = &pTrade->m_List[i];

			//	Service

			pCommodity->iService = serviceNone;
			for (j = 0; j < serviceCount; j++)
				{
				if (strEquals(pLine->GetTag(), CString(SERVICE_DATA[j].pszTag, -1, true)))
					{
					pCommodity->iService = (ETradeServiceTypes)j;
					break;
					}
				}

			if (pCommodity->iService == serviceNone)
				{
				Ctx.sError = strPatternSubst(CONSTLIT("Unknown Trade directive: %s."), pLine->GetTag());
				return ERR_FAIL;
				}

			//	Get the criteria. Depending on the service this might be either
			//	an item criteria or a type criteria.

			CString sCriteria = pLine->GetAttribute(CRITERIA_ATTRIB);

			//	Depending on the service we get a slightly different set of 
			//	parameters.
			//
			//	For selling ships, we need the type criteria.

			if (pCommodity->iService == serviceBuyShip
					|| pCommodity->iService == serviceSellShip)
				{
				if (error = CDesignTypeCriteria::ParseCriteria(sCriteria, &pCommodity->TypeCriteria))
					{
					Ctx.sError = strPatternSubst(CONSTLIT("Unable to parse criteria: %s."), sCriteria);
					return ERR_FAIL;
					}
				}

			//	Otherwise, we want item criteria

			else
				{
				if (!sCriteria.IsBlank())
					CItem::ParseCriteria(sCriteria, &pCommodity->ItemCriteria);
				else
					CItem::InitCriteriaAll(&pCommodity->ItemCriteria);

				//	Item

				if (error = pCommodity->pItemType.LoadUNID(Ctx, pLine->GetAttribute(ITEM_ATTRIB)))
					return error;

				//	Some items are replenished over time.

				if (error = pCommodity->InventoryAdj.InitFromString(Ctx, pLine->GetAttribute(INVENTORY_ADJ_ATTRIB)))
					return error;
				}

			//	Level frequency

			pCommodity->sLevelFrequency = pLine->GetAttribute(LEVEL_FREQUENCY_ATTRIB);

			//	Other

			if (error = pCommodity->PriceAdj.InitFromString(Ctx, pLine->GetAttribute(PRICE_ADJ_ATTRIB)))
				return error;

			//	Message ID

			pCommodity->sMessageID = pLine->GetAttribute(MESSAGE_ID_ATTRIB);

			//	Flags

			pCommodity->dwFlags = 0;
			if (pLine->GetAttributeBool(ACTUAL_PRICE_ATTRIB))
				pCommodity->dwFlags |= FLAG_ACTUAL_PRICE;

			if (!pCommodity->InventoryAdj.IsEmpty())
				pCommodity->dwFlags |= FLAG_INVENTORY_ADJ;

			if (pLine->GetAttributeBool(UPGRADE_INSTALL_ONLY_ATTRIB))
				pCommodity->dwFlags |= FLAG_UPGRADE_INSTALL_ONLY;

            if (pLine->GetAttributeBool(NO_DESCRIPTION_ATTRIB))
                pCommodity->dwFlags |= FLAG_NO_DESCRIPTION;

			//	Set ID

			pCommodity->sID = pTrade->ComputeID(pCommodity->iService, pCommodity->pItemType.GetUNID(), sCriteria, pCommodity->dwFlags);
			}
		}

	//	Done

	*retpTrade = pTrade;

	return NOERROR;
	}

bool CTradingDesc::Buys (CSpaceObject *pObj, const CItem &Item, DWORD dwFlags, int *retiPrice, int *retiMaxCount)

//	Buys
//
//	Returns TRUE if the given object buys items of the given type.
//	Optionally returns a price and a max number.
//
//	Note that we always return a price for items we are willing to buy, even if we
//	don't currently have enough to buy it.

	{
	int i;

	STradeServiceCtx Ctx;
	Ctx.pProvider = pObj;
	Ctx.pCurrency = m_pCurrency;
	Ctx.pItem = &Item;
	Ctx.iCount = 1;

	//	Loop over the commodity list and find the first entry that matches

	for (i = 0; i < m_List.GetCount(); i++)
		if (((m_List[i].iService == serviceBuy) || (m_List[i].iService == serviceAcceptDonations))
				&& Matches(Item, m_List[i]))
			{
			//	If this is virtual price and we don't want virtual, then skip

			if ((m_List[i].iService == serviceAcceptDonations)
					&& (dwFlags & FLAG_NO_DONATION))
				return false;

			//	Set the service

			Ctx.iService = m_List[i].iService;

			//	Compute price

			int iPrice = ComputePrice(Ctx, m_List[i], dwFlags);
			if (iPrice < 0)
				return false;

			//	Compute the maximum number of this item that we are willing
			//	to buy. First we figure out how much money the station has left

			int iBalance = (int)pObj->GetBalance(m_pCurrency->GetUNID());
			int iMaxCount = (iPrice > 0 ? (iBalance / iPrice) : 0);

			//	Done

			if (retiMaxCount)
				*retiMaxCount = iMaxCount;

			if (retiPrice)
				*retiPrice = iPrice;

			return true;
			}

	return false;
	}

bool CTradingDesc::BuysShip (CSpaceObject *pObj, CSpaceObject *pShip, DWORD dwFlags, int *retiPrice)

//	BuysShip
//
//	Returns TRUE if the given object can buys the given ship. Optionally returns
//	a price.

	{
	STradeServiceCtx Ctx;
	Ctx.iService = serviceBuyShip;
	Ctx.pProvider = pObj;
	Ctx.pCurrency = m_pCurrency;
	Ctx.pObj = pShip;
	Ctx.iCount = 1;

	//	Compute price

	int iPrice = ComputePrice(Ctx, dwFlags);
	if (iPrice <= 0)
		return false;

	//	Done

	if (retiPrice)
		*retiPrice = iPrice;

	return true;
	}

int CTradingDesc::Charge (CSpaceObject *pObj, int iCharge)

//	Charge
//
//	Charge out of the station's balance

	{
	if (m_iMaxCurrency)
		return (int)pObj->ChargeMoney(m_pCurrency->GetUNID(), iCharge);
	else
		return 0;
	}

bool CTradingDesc::FindService (ETradeServiceTypes iService, const CItem &Item, const SServiceDesc **retpDesc)

//	FindService
//
//	Finds the given service for the given item.

	{
	int i;

	//	Loop over the commodity list and find the first entry that matches

	for (i = 0; i < m_List.GetCount(); i++)
		if (m_List[i].iService == iService 
				&& Matches(Item, m_List[i]))
			{
			if (retpDesc)
				*retpDesc = &m_List[i];

			return true;
			}
	
	return false;
	}

bool CTradingDesc::FindService (ETradeServiceTypes iService, CSpaceObject *pShip, const SServiceDesc **retpDesc)

//	FindService
//
//	Finds the given service for the given item.

	{
	int i;

	//	Loop over the commodity list and find the first entry that matches

	for (i = 0; i < m_List.GetCount(); i++)
		if (m_List[i].iService == iService 
				&& Matches(pShip->GetType(), m_List[i]))
			{
			if (retpDesc)
				*retpDesc = &m_List[i];

			return true;
			}
	
	return false;
	}

bool CTradingDesc::GetArmorInstallPrice (CSpaceObject *pObj, const CItem &Item, DWORD dwFlags, int *retiPrice, CString *retsReason) const

//	GetArmorInstallPrice
//
//	Returns the price to install the given armor

	{
	int i;

	STradeServiceCtx Ctx;
	Ctx.iService = serviceReplaceArmor;
	Ctx.pProvider = pObj;
	Ctx.pCurrency = m_pCurrency;
	Ctx.pItem = &Item;
	Ctx.iCount = 1;

	//	Loop over the commodity list and find the first entry that matches

	for (i = 0; i < m_List.GetCount(); i++)
		if (m_List[i].iService == serviceReplaceArmor
				&& Matches(Item, m_List[i]))
			{
			//	Compute price

			int iPrice = ComputePrice(Ctx, m_List[i], dwFlags);
			if (iPrice < 0)
				{
				if (retsReason)
					*retsReason = m_List[i].sMessageID;
				return false;
				}

			//	Done

			if (retiPrice)
				*retiPrice = iPrice;

			if (retsReason)
				*retsReason = m_List[i].sMessageID;

			return true;
			}

	return false;
	}

bool CTradingDesc::GetArmorRepairPrice (CSpaceObject *pObj, CSpaceObject *pSource, const CItem &Item, int iHPToRepair, DWORD dwFlags, int *retiPrice) const

//	GetArmorRepairPrice
//
//	Returns the price for repairing the given armor.

	{
	int i;

	STradeServiceCtx Ctx;
	Ctx.iService = serviceRepairArmor;
	Ctx.pProvider = pObj;
	Ctx.pCurrency = m_pCurrency;
    Ctx.pObj = pSource;
	Ctx.pItem = &Item;
	Ctx.iCount = iHPToRepair;

	//	Loop over the commodity list and find the first entry that matches

	for (i = 0; i < m_List.GetCount(); i++)
		if (m_List[i].iService == serviceRepairArmor
				&& Matches(Item, m_List[i]))
			{
			//	Compute price

			int iPrice = ComputePrice(Ctx, m_List[i], dwFlags);
			if (iPrice < 0)
				return false;

			//	Done

			if (retiPrice)
				*retiPrice = iPrice;

			return true;
			}

	return false;
	}

bool CTradingDesc::GetDeviceInstallPrice (CSpaceObject *pObj, const CItem &Item, DWORD dwFlags, int *retiPrice, CString *retsReason, DWORD *retdwPriceFlags) const

//	GetDeviceInstallPrice
//
//	Returns the price to install the given device

	{
	int i;

	STradeServiceCtx Ctx;
	Ctx.iService = serviceInstallDevice;
	Ctx.pProvider = pObj;
	Ctx.pCurrency = m_pCurrency;
	Ctx.pItem = &Item;
	Ctx.iCount = 1;

	//	Loop over the commodity list and find the first entry that matches

	for (i = 0; i < m_List.GetCount(); i++)
		if (m_List[i].iService == serviceInstallDevice
				&& Matches(Item, m_List[i]))
			{
			//	Compute price

			int iPrice = ComputePrice(Ctx, m_List[i], dwFlags);
			if (iPrice < 0)
				{
				if (retsReason)
					*retsReason = m_List[i].sMessageID;
				return false;
				}

			//	Done

			if (retiPrice)
				*retiPrice = iPrice;

			if (retsReason)
				*retsReason = m_List[i].sMessageID;

			if (retdwPriceFlags)
				{
				(*retdwPriceFlags) = 0;
				if (m_List[i].dwFlags & FLAG_UPGRADE_INSTALL_ONLY)
					(*retdwPriceFlags) |= PRICE_UPGRADE_INSTALL_ONLY;
				}

			return true;
			}

	return false;
	}

bool CTradingDesc::GetDeviceRemovePrice (CSpaceObject *pObj, const CItem &Item, DWORD dwFlags, int *retiPrice, DWORD *retdwPriceFlags) const

//	GetDeviceRemovePrice
//
//	Returns the price to remove the given device

	{
	int i;

	STradeServiceCtx Ctx;
	Ctx.iService = serviceRemoveDevice;
	Ctx.pProvider = pObj;
	Ctx.pCurrency = m_pCurrency;
	Ctx.pItem = &Item;
	Ctx.iCount = 1;

	//	Loop over the commodity list and find the first entry that matches

	for (i = 0; i < m_List.GetCount(); i++)
		if (m_List[i].iService == serviceRemoveDevice
				&& Matches(Item, m_List[i]))
			{
			//	Compute price

			int iPrice = ComputePrice(Ctx, m_List[i], dwFlags);
			if (iPrice < 0)
				return false;

			//	Done

			if (retiPrice)
				*retiPrice = iPrice;

			if (retdwPriceFlags)
				{
				(*retdwPriceFlags) = 0;
				if (m_List[i].dwFlags & FLAG_UPGRADE_INSTALL_ONLY)
					(*retdwPriceFlags) |= PRICE_UPGRADE_INSTALL_ONLY;
				}

			return true;
			}

	return false;
	}

int CTradingDesc::GetMaxLevelMatched (ETradeServiceTypes iService, bool bDescriptionOnly) const

//	GetMaxLevelMatched
//
//	Returns the max item level matched for the given service. Returns -1 if the
//	service is not available.

	{
	int i;
	int iMaxLevel = -1;

	//	Loop over the commodity list and find the first entry that matches

	for (i = 0; i < m_List.GetCount(); i++)
		if (m_List[i].iService == iService)
			{
			CString sPrefix;
			int iPriceAdj = m_List[i].PriceAdj.EvalAsInteger(NULL, &sPrefix);
			if (strEquals(sPrefix, UNAVAILABLE_PREFIX))
				continue;

            //  If we're looking for a description, ignore any entries that
            //  don't want one.

            if (bDescriptionOnly && (m_List[i].dwFlags & FLAG_NO_DESCRIPTION))
                continue;

			int iLevel;
			if (m_List[i].pItemType)
				iLevel = m_List[i].pItemType->GetLevel();
			else
				iLevel = m_List[i].ItemCriteria.GetMaxLevelMatched();

			if (iLevel > iMaxLevel)
				iMaxLevel = iLevel;
			}
	
	return iMaxLevel;
	}

bool CTradingDesc::GetRefuelItemAndPrice (CSpaceObject *pObj, CSpaceObject *pObjToRefuel, DWORD dwFlags, CItemType **retpItemType, int *retiPrice) const

//	GetRefuelItemAndPrice
//
//	Returns the appropriate fuel and price to refuel the given object.

	{
	int i, j;
	CShip *pShipToRefuel = pObjToRefuel->AsShip();
	if (pShipToRefuel == NULL)
		return false;

	STradeServiceCtx Ctx;
	Ctx.iService = serviceRefuel;
	Ctx.pProvider = pObj;
	Ctx.pCurrency = m_pCurrency;
	Ctx.iCount = 1;

	Metric rMaxFuel = pShipToRefuel->GetMaxFuel();

	for (i = 0; i < m_List.GetCount(); i++)
		if (m_List[i].iService == serviceRefuel)
			{
			int iBestPrice = 0;
			int iBestMetric = 0;
			CItemType *pBestItem = NULL;

			//	Find the highest-level item that matches the given criteria.
			//	If we find it, then we use it.

			for (j = 0; j < g_pUniverse->GetItemTypeCount(); j++)
				{
				CItemType *pType = g_pUniverse->GetItemType(j);
				CItem Item(pType, 1);

				if (Item.MatchesCriteria(m_List[i].ItemCriteria)
						&& pShipToRefuel->IsFuelCompatible(Item))
					{
					//	Compute how good this fuel is relative to others. Any fuel 
					//	that requires at least 10 items to fill the ship is worth
					//	100 points.
					//
					//	We add the item level to that base. We need to do this because
					//	we don't want to refuel with in big chunks (or else we'll
					//	think the ship is close enough to full).

					Metric rFuelPerItem = Max(1.0, strToDouble(Item.GetType()->GetData(), 0.0, NULL));
					Metric rItemsToFillShip = rMaxFuel / rFuelPerItem;

					int iMetric = ((rItemsToFillShip >= 10.0) ? 100 : 0) + pType->GetLevel();
					if (pBestItem == NULL || iMetric > iBestMetric)
						{
						//	Compute the price, because if we don't sell it, then we
						//	skip it.
						//
						//	NOTE: Unlike selling, we allow 0 prices because some 
						//	stations give fuel for free.

						Ctx.pItem = &Item;
						
						int iPrice = ComputePrice(Ctx, m_List[i], dwFlags);
						if (iPrice >= 0)
							{
							pBestItem = pType;
							iBestPrice = iPrice;
							iBestMetric = iMetric;
							}
						}
					}
				}

			//	If found, then return it.

			if (pBestItem)
				{
				if (retpItemType)
					*retpItemType = pBestItem;

				if (retiPrice)
					*retiPrice = iBestPrice;

				return true;
				}
			}

	//	Otherwise, not found

	return false;
	}

bool CTradingDesc::GetServiceInfo (ETradeServiceTypes iService, SServiceInfo &Info) const

//  GetServiceInfo
//
//  Returns information about the given service suitable for creating a human-
//  readable description.

    {
	int i;

    //  Initialize to defaults.

    Info.bAvailable = false;
    Info.bUpdateInstallOnly = true;
    Info.iMaxLevel = -1;

	//	Loop over the commodity list and find the first entry that matches

	for (i = 0; i < m_List.GetCount(); i++)
		if (m_List[i].iService == iService)
			{
			CString sPrefix;
			int iPriceAdj = m_List[i].PriceAdj.EvalAsInteger(NULL, &sPrefix);

            //  Ignore entries that don't count towards the description

			if (strEquals(sPrefix, UNAVAILABLE_PREFIX)
                    || (m_List[i].dwFlags & FLAG_NO_DESCRIPTION))
				continue;

            //  Found at least one service.

            Info.bAvailable = true;

            //  If we don't require a purchase, then clear the flag

            if (!(m_List[i].dwFlags & FLAG_UPGRADE_INSTALL_ONLY))
                Info.bUpdateInstallOnly = false;

            //  Compute the level

			int iLevel;
			if (m_List[i].pItemType)
				iLevel = m_List[i].pItemType->GetLevel();
			else
				iLevel = m_List[i].ItemCriteria.GetMaxLevelMatched();

			if (iLevel > Info.iMaxLevel)
				Info.iMaxLevel = iLevel;
			}

	return Info.bAvailable;
    }

bool CTradingDesc::HasService (ETradeServiceTypes iService) const

//	HasService
//
//	Returns true if it has the given service.

	{
	int i;

	//	Loop over the commodity list and find the first entry that matches

	for (i = 0; i < m_List.GetCount(); i++)
		if (m_List[i].iService == iService)
			{
			CString sPrefix;
			int iPriceAdj = m_List[i].PriceAdj.EvalAsInteger(NULL, &sPrefix);
			if (strEquals(sPrefix, UNAVAILABLE_PREFIX))
				continue;

            return true;
			}
	
	return false;
	}

bool CTradingDesc::HasServiceUpgradeOnly (ETradeServiceTypes iService) const

//	HasServiceUpgradeOnly
//
//	Returns TRUE if we have the service, but for upgrade only.

	{
	int i;

	bool bHasUpgrade = false;
	bool bHasNonUpgrade = false;

	//	Loop over the commodity list and find the first entry that matches

	for (i = 0; i < m_List.GetCount(); i++)
		if (m_List[i].iService == iService)
			{
			CString sPrefix;
			int iPriceAdj = m_List[i].PriceAdj.EvalAsInteger(NULL, &sPrefix);
			if (strEquals(sPrefix, UNAVAILABLE_PREFIX))
				continue;

			if (m_List[i].dwFlags & FLAG_UPGRADE_INSTALL_ONLY)
				bHasUpgrade = true;
			else
				bHasNonUpgrade = true;
			}
	
	return (bHasUpgrade && !bHasNonUpgrade);
	}

bool CTradingDesc::HasServiceDescription (ETradeServiceTypes iService) const

//	HasServiceDescription
//
//	Returns TRUE if it has the given service AND we should add it to the 
//  service description (in ComposeDescription).

	{
	int i;

	//	Loop over the commodity list and find the first entry that matches

	for (i = 0; i < m_List.GetCount(); i++)
		if (m_List[i].iService == iService)
			{
			CString sPrefix;
			int iPriceAdj = m_List[i].PriceAdj.EvalAsInteger(NULL, &sPrefix);
			if (strEquals(sPrefix, UNAVAILABLE_PREFIX)
                    || (m_List[i].dwFlags & FLAG_NO_DESCRIPTION))
				continue;

            return true;
			}
	
	return false;
	}

bool CTradingDesc::Matches (const CItem &Item, const SServiceDesc &Commodity) const

//	Matches
//
//	Returns TRUE if the given item matches the commodity

	{
	if (Commodity.pItemType)
		return (Commodity.pItemType == Item.GetType());
	else
		return Item.MatchesCriteria(Commodity.ItemCriteria);
	}

bool CTradingDesc::Matches (CDesignType *pType, const SServiceDesc &Commodity) const

//	Matches
//
//	Returns TRUE if the given type matches the commodity

	{
	if (pType == NULL)
		return false;

	return pType->MatchesCriteria(Commodity.TypeCriteria);
	}

void CTradingDesc::OnCreate (CSpaceObject *pObj)

//	OnCreate
//
//	Station is created

	{
	//	Give the station a limited amount of money

	if (m_iMaxCurrency)
		{
		int iMaxCurrency = ComputeMaxCurrency(pObj);
		pObj->CreditMoney(m_pCurrency->GetUNID(), iMaxCurrency);
		}
	}

ALERROR CTradingDesc::OnDesignLoadComplete (SDesignLoadCtx &Ctx)

//	OnDesignLoadComplete
//
//	Design loaded

	{
	ALERROR error;
	int i;

	if (error = m_pCurrency.Bind(Ctx))
		return error;

	for (i = 0; i < m_List.GetCount(); i++)
		if (error = m_List[i].pItemType.Bind(Ctx))
			return error;

	return NOERROR;
	}

void CTradingDesc::OnUpdate (CSpaceObject *pObj)

//	OnUpdate
//
//	Station updates (call roughly every 1800 ticks)

	{
	DEBUG_TRY

	if (m_iMaxCurrency && m_iReplenishCurrency)
		{
		int iBalance = (int)pObj->GetBalance(m_pCurrency->GetUNID());
		int iMaxCurrency = ComputeMaxCurrency(pObj);

		if (iBalance < iMaxCurrency)
			pObj->CreditMoney(m_pCurrency->GetUNID(), m_iReplenishCurrency);
		}

	DEBUG_CATCH
	}

void CTradingDesc::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Read from a stream
//
//	DWORD			m_pCurrency UNID
//	DWORD			m_iMaxCurrency
//	DWORD			m_iReplenishCurrency
//	DWORD			No of orders
//
//	DWORD			iService
//	CString			sID
//	DWORD			pItemType
//	CString			ItemCriteria or TypeCriteria
//	CFormulaText	PriceAdj
//	CFormulaText	InventoryAdj
//	CString			sMessageID
//	DWORD			dwFlags

	{
	int i;
	DWORD dwLoad;

	if (Ctx.dwVersion >= 62)
		{
		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
		m_pCurrency.Set(dwLoad);
		if (m_pCurrency == NULL)
			m_pCurrency.Set(DEFAULT_ECONOMY_UNID);
		}
	else
		{
		CString sDummy;
		sDummy.ReadFromStream(Ctx.pStream);
		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));

		//	Previous versions are always credits

		m_pCurrency.Set(DEFAULT_ECONOMY_UNID);
		}

	Ctx.pStream->Read((char *)&m_iMaxCurrency, sizeof(DWORD));
	Ctx.pStream->Read((char *)&m_iReplenishCurrency, sizeof(DWORD));

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	if (dwLoad > 0)
		{
		m_List.InsertEmpty(dwLoad);

		for (i = 0; i < m_List.GetCount(); i++)
			{
			SServiceDesc &Commodity = m_List[i];

			if (Ctx.dwVersion >= 83)
				Ctx.pStream->Read((char *)&Commodity.iService, sizeof(DWORD));

			//	Temporarily initialize because we might check it below before we
			//	can properly load this.

			else
				Commodity.iService = serviceNone;

			Commodity.sID.ReadFromStream(Ctx.pStream);

			Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
			Commodity.pItemType = g_pUniverse->FindItemType(dwLoad);

			CString sCriteria;
			sCriteria.ReadFromStream(Ctx.pStream);

			//	For ships, this is a type criteria

			if (Commodity.iService == serviceBuyShip
					|| Commodity.iService == serviceSellShip)
				{
				CDesignTypeCriteria::ParseCriteria(sCriteria, &Commodity.TypeCriteria);
				CItem::InitCriteriaAll(&Commodity.ItemCriteria);
				}

			//	Otherwise, this is an item criteria

			else
				CItem::ParseCriteria(sCriteria, &Commodity.ItemCriteria);

			//	Prices

			if (Ctx.dwVersion >= 62)
				{
				Commodity.PriceAdj.ReadFromStream(Ctx);
				Commodity.InventoryAdj.ReadFromStream(Ctx);
				}
			else
				{
				Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
				Commodity.PriceAdj.SetInteger(dwLoad);
				}

			if (Ctx.dwVersion >= 142)
				Commodity.sLevelFrequency.ReadFromStream(Ctx.pStream);

			if (Ctx.dwVersion >= 113)
				Commodity.sMessageID.ReadFromStream(Ctx.pStream);

			Ctx.pStream->Read((char *)&Commodity.dwFlags, sizeof(DWORD));

			//	If necessary we need to load backwards-compatible service

			if (Ctx.dwVersion < 83)
				ReadServiceFromFlags(Commodity.dwFlags, &Commodity.iService, &Commodity.dwFlags);

			//	For now we don't support inventory adj in dynamic trade descs
			
			Commodity.dwFlags &= ~FLAG_INVENTORY_ADJ;
			}
		}
	}

void CTradingDesc::ReadServiceFromFlags (DWORD dwFlags, ETradeServiceTypes *retiService, DWORD *retdwFlags)

//	ReadServiceFromFlags
//
//	Before we had service types we encoded info in flags. This function converts
//	to the new method.

	{
	if (retiService)
		{
		if (dwFlags & FLAG_SELLS)
			*retiService = serviceSell;
		else if (dwFlags & FLAG_BUYS)
			*retiService = serviceBuy;
		else if (dwFlags & FLAG_ACCEPTS_DONATIONS)
			*retiService = serviceAcceptDonations;
		else
			{
			ASSERT(false);
			*retiService = serviceNone;
			}
		}

	if (retdwFlags)
		*retdwFlags = (dwFlags & ~(FLAG_SELLS | FLAG_BUYS | FLAG_ACCEPTS_DONATIONS));
	}

void CTradingDesc::RefreshInventory (CSpaceObject *pObj, int iPercent)

//	RefreshInventory
//
//	Refresh the inventory for all entries that have an inventory
//	adjustment factor.

	{
	DEBUG_TRY

	int i, j;
	bool bCargoChanged = false;

	for (i = 0; i < m_List.GetCount(); i++)
		{
		SServiceDesc &Service = m_List[i];
		if (!(Service.dwFlags & FLAG_INVENTORY_ADJ))
			continue;

		//	See if we have a level frequency.

		CString sLevelFrequency = Service.sLevelFrequency;
		if (strFind(sLevelFrequency, CONSTLIT(":")) != -1)
			sLevelFrequency = GenerateLevelFrequency(sLevelFrequency, pObj->GetSystem()->GetLevel());

		//	Make a list of all item types that match the given
		//	criteria.

		TArray<CItemType *> ItemTable;
		for (j = 0; j < g_pUniverse->GetItemTypeCount(); j++)
			{
			CItemType *pType = g_pUniverse->GetItemType(j);
			CItem theItem(pType, 1);
			if (theItem.MatchesCriteria(Service.ItemCriteria))
				ItemTable.Insert(pType);

			//	Skip if we some percentage of items if we're not refreshing the
			//	entire inventory.

			if (iPercent < 100 && mathRandom(1, 100) > iPercent)
				continue;
			}

		//	Loop over the count

		if (ItemTable.GetCount() == 0)
			continue;

		//	Loop over all items refreshing them

		for (j = 0; j < ItemTable.GetCount(); j++)
			{
			//	Compute the probability based on the item frequency.

			int iChance = 100 * ItemTable[j]->GetFrequency() / ftCommon;

			//	If the refresh percent is greater than 100, then we multiply the probability.

			if (iPercent > 100)
				iChance = iPercent * iChance / 100;

			//	Adjust based on level, if necessary.

			if (!sLevelFrequency.IsBlank())
				iChance = iChance * GetFrequencyByLevel(sLevelFrequency, ItemTable[j]->GetLevel()) / ftCommon;

			//	Roll

			if (iChance >= 100 || mathRandom(1, 100) <= iChance)
				{
				if (SetInventoryCount(pObj, Service, ItemTable[j]))
					bCargoChanged = true;
				}
			}
		}

	if (bCargoChanged)
		pObj->ItemsModified();

	DEBUG_CATCH
	}

bool CTradingDesc::Sells (CSpaceObject *pObj, const CItem &Item, DWORD dwFlags, int *retiPrice)

//	Sells
//
//	Returns TRUE if the given object can currently sell the given item type.
//	Optionally returns a price

	{
	STradeServiceCtx Ctx;
	Ctx.iService = serviceSell;
	Ctx.pProvider = pObj;
	Ctx.pCurrency = m_pCurrency;
	Ctx.pItem = &Item;
	Ctx.iCount = 1;

	//	Compute price

	int iPrice = ComputePrice(Ctx, dwFlags);
	if (iPrice <= 0)
		return false;

	//	Done

	if (retiPrice)
		*retiPrice = iPrice;

	return true;
	}

bool CTradingDesc::SellsShip (CSpaceObject *pObj, CSpaceObject *pShip, DWORD dwFlags, int *retiPrice)

//	SellsShip
//
//	Returns TRUE if the given object can sell the given ship. Optionally returns
//	a price.

	{
	STradeServiceCtx Ctx;
	Ctx.iService = serviceSellShip;
	Ctx.pProvider = pObj;
	Ctx.pCurrency = m_pCurrency;
	Ctx.pObj = pShip;
	Ctx.iCount = 1;

	//	Compute price

	int iPrice = ComputePrice(Ctx, dwFlags);
	if (iPrice <= 0)
		return false;

	//	Done

	if (retiPrice)
		*retiPrice = iPrice;

	return true;
	}

CString CTradingDesc::ServiceToString (ETradeServiceTypes iService)

//	ServiceToString
//
//	Returns a string representing the service

	{
	if (iService >= 0 && iService < serviceCount)
		return CString(SERVICE_DATA[iService].pszName);
	else
		return CONSTLIT("unknown");
	}

bool CTradingDesc::SetInventoryCount (CSpaceObject *pObj, SServiceDesc &Desc, CItemType *pItemType)

//	SetInventoryCount
//
//	Sets the count for the given item
//	Returns TRUE if the count was changed; FALSE otherwise.

	{
	bool bCargoChanged = false;
	CItemListManipulator ItemList(pObj->GetItemList());

	//	Roll number appearing

	int iItemCount = pItemType->GetNumberAppearing().Roll();

	//	Adjust based on inventory adjustment

	iItemCount = iItemCount * Desc.InventoryAdj.EvalAsInteger(pObj) / 100;

	//	If the item exists, set the count

	if (ItemList.SetCursorAtItem(CItem(pItemType, 1)))
		{
		ItemList.SetCountAtCursor(iItemCount);
		bCargoChanged = true;
		}

	//	Otherwise, add the appropriate number of items

	else if (iItemCount > 0)
		{
		ItemList.AddItem(CItem(pItemType, iItemCount));
		bCargoChanged = true;
		}

	//	Done

	return bCargoChanged;
	}

void CTradingDesc::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	Writes to a stream
//
//	DWORD			m_pCurrency UNID
//	DWORD			m_iMaxCurrency
//	DWORD			m_iReplenishCurrency
//	DWORD			No of orders
//
//	DWORD			iService
//	CString			sID
//	DWORD			pItemType
//	CString			ItemCriteria or TypeCriteria
//	CFormulaText	PriceAdj
//	CFormulaText	InventoryAdj
//	CString			sLevelFrequency
//	CString			sMessageID
//	DWORD			dwFlags

	{
	int i;
	DWORD dwSave;

	dwSave = (m_pCurrency ? m_pCurrency->GetUNID() : 0);
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	pStream->Write((char *)&m_iMaxCurrency, sizeof(DWORD));
	pStream->Write((char *)&m_iReplenishCurrency, sizeof(DWORD));

	dwSave = m_List.GetCount();
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	for (i = 0; i < m_List.GetCount(); i++)
		{
		const SServiceDesc &Commodity = m_List[i];

		dwSave = Commodity.iService;
		pStream->Write((char *)&dwSave, sizeof(DWORD));

		Commodity.sID.WriteToStream(pStream);

		dwSave = (Commodity.pItemType ? Commodity.pItemType->GetUNID() : 0);
		pStream->Write((char *)&dwSave, sizeof(DWORD));

		//	Criteria is different depending on service

		CString sCriteria;
		if (Commodity.iService == serviceBuyShip
				|| Commodity.iService == serviceSellShip)
			sCriteria = Commodity.TypeCriteria.AsString();
		else
			sCriteria = CItem::GenerateCriteria(Commodity.ItemCriteria);

		sCriteria.WriteToStream(pStream);

		Commodity.PriceAdj.WriteToStream(pStream);
		Commodity.InventoryAdj.WriteToStream(pStream);
		Commodity.sLevelFrequency.WriteToStream(pStream);

		Commodity.sMessageID.WriteToStream(pStream);

		pStream->Write((char *)&Commodity.dwFlags, sizeof(DWORD));
		}
	}

//  CTradingDesc::SServiceDesc -------------------------------------------------

CTradingDesc::SServiceDesc::~SServiceDesc (void)

//  SServiceDesc destructor

    {
    if (pPriceAdjCode)
        pPriceAdjCode->Discard(&g_pUniverse->GetCC());
    }
