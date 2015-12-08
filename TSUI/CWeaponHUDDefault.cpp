//	CWeaponHUDDefault.cpp
//
//	CWeaponHUDDefault class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

#define IMAGE_TAG							CONSTLIT("Image")

#define STR_UNKNOWN_HOSTILE					CONSTLIT("Unknown Hostile")
#define STR_UNKNOWN_FRIENDLY				CONSTLIT("Unknown Friendly")

#define ERR_WEAPON_DISPLAY_NEEDED			CONSTLIT("invalid <WeaponDisplay> element")

const int DISPLAY_WIDTH =					360;
const int DISPLAY_HEIGHT =					120;

const int PRIMARY_WEAPON_X =				300;
const int PRIMARY_WEAPON_Y =				80;
const int MISSILE_WEAPON_X =				300;
const int MISSILE_WEAPON_Y =				100;

const int DEVICE_STATUS_HEIGHT =			20;

const int TARGET_IMAGE_X =					60;
const int TARGET_IMAGE_Y =					60;
const int TARGET_NAME_X =					122;
const int TARGET_NAME_Y =					27;

const CG32bitPixel DISABLED_LABEL_COLOR =	CG32bitPixel(128, 0, 0);
const CG32bitPixel TARGET_NAME_COLOR =		CG32bitPixel(5, 211, 5);

CWeaponHUDDefault::CWeaponHUDDefault (void) :
		m_bInvalid(true),
		m_pDefaultBack(NULL)

//	CWeaponHUDDefault constructor

	{
	}

CWeaponHUDDefault::~CWeaponHUDDefault (void)

//	CWeaponHUDDefault destructor

	{
	}

ALERROR CWeaponHUDDefault::Bind (SDesignLoadCtx &Ctx)

//	Bind
//
//	Bind design

	{
	ALERROR error;

	if (error = m_BackImage.OnDesignLoadComplete(Ctx))
		return error;

	m_pDefaultBack = NULL;

	return NOERROR;
	}

void CWeaponHUDDefault::GetBounds (int *retWidth, int *retHeight) const

//	GetBounds
//
//	Returns the bounds of the HUD

	{
	*retWidth = DISPLAY_WIDTH;
	*retHeight = DISPLAY_HEIGHT;
	}

ALERROR CWeaponHUDDefault::InitFromXML (SDesignLoadCtx &Ctx, CShipClass *pClass, CXMLElement *pDesc)

//	InitFromXML
//
//	Initializes from XML descriptor

	{
	ALERROR error;

	//	Load the image

	if (error = m_BackImage.InitFromXML(Ctx, 
			pDesc->GetContentElementByTag(IMAGE_TAG)))
		return ComposeLoadError(Ctx, ERR_WEAPON_DISPLAY_NEEDED);

	//	Done

	return NOERROR;
	}

void CWeaponHUDDefault::OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx)

//	OnPaint
//
//	Paint

	{
	if (m_bInvalid)
		{
		Realize(Ctx);
		m_bInvalid = false;
		}

	Dest.Blt(0,
			0,
			m_Buffer.GetWidth(),
			m_Buffer.GetHeight(),
			200,
			m_Buffer,
			x,
			y);
	}

void CWeaponHUDDefault::PaintDeviceStatus (CShip *pShip, DeviceNames iDev, int x, int y)

//	PaintDeviceStatus
//
//	Paints the status and ammo for a device

	{
	const CVisualPalette &VI = g_pHI->GetVisuals();
	const CG16bitFont &SmallFont = VI.GetFont(fontSmall);
	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);
	const CG16bitFont &LargeBoldFont = VI.GetFont(fontLargeBold);

	CInstalledDevice *pDevice = pShip->GetNamedDevice(iDev);
	if (pDevice)
		{
		CDeviceClass *pClass = pDevice->GetClass();

		CString sVariant;
		int iAmmoLeft;
		pClass->GetSelectedVariantInfo(pShip, pDevice, &sVariant, &iAmmoLeft);
		CString sDevName = pClass->GetName();

		//	Paint the bonus

		int cxBonus = 0;

		CString sBonus = pDevice->GetEnhancedDesc(pShip);
		if (!sBonus.IsBlank())
			{
			int cyHeight;
			cxBonus = SmallFont.MeasureText(sBonus, &cyHeight);

			//	Background

			RECT rcRect;
			rcRect.left = x - cxBonus - 8;
			rcRect.right = rcRect.left + cxBonus;
			rcRect.top = y + (DEVICE_STATUS_HEIGHT - cyHeight) / 2;
			rcRect.bottom = rcRect.top + cyHeight;

			bool bDisadvantage = (*(sBonus.GetASCIIZPointer()) == '-');
			m_Buffer.Fill(rcRect.left - 2, 
					rcRect.top + 1, 
					cxBonus + 4, 
					cyHeight - 2, 
					(bDisadvantage ? VI.GetColor(colorAreaDisadvantage) : VI.GetColor(colorAreaAdvantage)));

			//	Bonus text

			SmallFont.DrawText(m_Buffer,
					rcRect.left,
					rcRect.top,
					(bDisadvantage ? VI.GetColor(colorTextDisadvantage) : VI.GetColor(colorTextAdvantage)),
					sBonus);

			cxBonus += 4;
			}

		//	Figure out what color to use

		CG32bitPixel rgbColor;
		if (pDevice->IsEnabled() && !pDevice->IsDamaged() && !pDevice->IsDisrupted())
			rgbColor = VI.GetColor(colorTextHighlight);
		else
			rgbColor = DISABLED_LABEL_COLOR;

		//	Paint the name

		if (sVariant.IsBlank())
			sVariant = sDevName;

		int cyHeight;
		int cxWidth = MediumFont.MeasureText(sVariant, &cyHeight);

		MediumFont.DrawText(m_Buffer,
				x - cxWidth - 8 - cxBonus,
				y + (DEVICE_STATUS_HEIGHT - cyHeight) / 2,
				rgbColor,
				sVariant);

		//	Paint the ammo counter

		if (iAmmoLeft != -1)
			{
			CString sAmmo = strFromInt(iAmmoLeft);
			LargeBoldFont.DrawText(m_Buffer,
					x,
					y,
					VI.GetColor(colorTextHighlight),
					sAmmo);
			}
		}
	}

void CWeaponHUDDefault::Realize (SHUDPaintCtx &Ctx)

//	Realize
//
//	Paint the m_Buffer

	{
	//	Skip if we don't have a ship

	CShip *pShip;
	if (Ctx.pSource == NULL
			|| (pShip = Ctx.pSource->AsShip()) == NULL)
		return;

	//	Set up some metrics

	const CVisualPalette &VI = g_pHI->GetVisuals();

	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);
	const CG16bitFont &MediumHeavyBoldFont = VI.GetFont(fontMediumHeavyBold);

	//	Create the buffer, if necessary

	if (m_Buffer.IsEmpty())
		m_Buffer.Create(DISPLAY_WIDTH, DISPLAY_HEIGHT, CG32bitImage::alpha8);

	m_Buffer.Set(CG32bitPixel::Null());

	//	Paint the background image

	if (!m_BackImage.IsEmpty())
		{
		const RECT &rcImage = m_BackImage.GetImageRect();

		m_Buffer.Blt(rcImage.left, 
				rcImage.top, 
				RectWidth(rcImage), 
				RectHeight(rcImage), 
				255,
				m_BackImage.GetImage(NULL_STR),
				0,
				0);
		}
	else
		{
		if (m_pDefaultBack == NULL)
			m_pDefaultBack = g_pUniverse->GetLibraryBitmap(UNID_TARGETING_HUD_BACKGROUND, CDesignCollection::FLAG_IMAGE_LOCK);

		if (m_pDefaultBack)
			m_Buffer.Blt(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, *m_pDefaultBack, 0, 0);
		}

	//	Draw the primary weapon status

	PaintDeviceStatus(pShip, 
			devPrimaryWeapon,
			PRIMARY_WEAPON_X,
			PRIMARY_WEAPON_Y);

	//	Draw the missile weapon status

	PaintDeviceStatus(pShip,
			devMissileWeapon,
			MISSILE_WEAPON_X,
			MISSILE_WEAPON_Y);

	//	Paint the target

	CSpaceObject *pTarget = pShip->GetTarget(CItemCtx(), true);
	if (pTarget)
		{
		//	Paint image

		if (pTarget->IsIdentified())
			{
			SViewportPaintCtx Ctx;
			Ctx.pObj = pTarget;
			Ctx.XForm = ViewportTransform(pTarget->GetPos(), 
					g_KlicksPerPixel, 
					TARGET_IMAGE_X, 
					TARGET_IMAGE_Y);
			Ctx.XFormRel = Ctx.XForm;
			Ctx.fNoRecon = true;
			Ctx.fNoDockedShips = true;
			Ctx.fNoSelection = true;

			pTarget->Paint(m_Buffer, TARGET_IMAGE_X, TARGET_IMAGE_Y, Ctx);

			//	Erase the area outside the target pane

			if (!m_BackImage.IsEmpty())
				{
				const RECT &rcImage = m_BackImage.GetImageRect();

				m_Buffer.CopyChannel(channelAlpha,
						rcImage.left, 
						rcImage.top, 
						RectWidth(rcImage), 
						RectHeight(rcImage), 
						m_BackImage.GetImage(NULL_STR),
						0,
						0);
				}
			else if (m_pDefaultBack)
				{
				m_Buffer.CopyChannel(channelAlpha,
						0, 
						0, 
						DISPLAY_WIDTH, 
						DISPLAY_HEIGHT, 
						*m_pDefaultBack,
						0,
						0);
				}
			}

		//	Paint the name of the target

		int x = TARGET_NAME_X;
		int y = TARGET_NAME_Y;

		CString sName;
		if (pTarget->IsIdentified())
			sName = pTarget->GetNounPhrase(nounCapitalize);
		else if (pShip->IsEnemy(pTarget))
			sName = STR_UNKNOWN_HOSTILE;
		else
			sName = STR_UNKNOWN_FRIENDLY;

		MediumHeavyBoldFont.DrawText(m_Buffer,
				x,
				y,
				CG32bitPixel::Darken(TARGET_NAME_COLOR, 180),
				sName);
		y += MediumHeavyBoldFont.GetHeight();

		//	Paint the range

		CVector vDist = pTarget->GetPos() - pShip->GetPos();
		int iDist = (int)((vDist.Length() / LIGHT_SECOND) + 0.5);
		CString sStatus = strPatternSubst(CONSTLIT("Range: %d"), iDist);
		MediumFont.DrawText(m_Buffer,
				x,
				y,
				TARGET_NAME_COLOR,
				sStatus);
		y += MediumFont.GetHeight();

		//	Paint the damage

		if (pTarget->IsIdentified())
			{
			int iDamage = pTarget->GetVisibleDamage();
			int iShields = pTarget->GetShieldLevel();
			if (iDamage > 0 || iShields > 0)
				{
				CString sDamage;
				
				if (iDamage == 0 && iShields > 0)
					sDamage = strPatternSubst(CONSTLIT("Shields: %d%%"), iShields);
				else if (iDamage > 0 && iShields <= 0)
					sDamage = strPatternSubst(CONSTLIT("Damage: %d%%"), iDamage);
				else if (iDamage > 0)
					sDamage = strPatternSubst(CONSTLIT("Damage: %d%%  Shields: %d%%"), iDamage, iShields);

				MediumFont.DrawText(m_Buffer,
						x,
						y,
						TARGET_NAME_COLOR,
						sDamage);
				}
			}
		}
	}