//	CDockingPortDesc.cpp
//
//	CDockingPortDesc class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define DOCKING_PORTS_TAG						CONSTLIT("DockingPorts")

#define BRING_TO_FRONT_ATTRIB					CONSTLIT("bringToFront")
#define DOCKING_PORTS_ATTRIB					CONSTLIT("dockingPorts")
#define MAX_DIST_ATTRIB							CONSTLIT("maxDist")
#define PORT_ANGLE_ATTRIB						CONSTLIT("portAngle")
#define PORT_COUNT_ATTRIB						CONSTLIT("portCount")
#define PORT_RADIUS_ATTRIB						CONSTLIT("portRadius")
#define POS_Z_ATTRIB							CONSTLIT("posZ")
#define ROTATION_ATTRIB							CONSTLIT("rotation")
#define SEND_TO_BACK_ATTRIB						CONSTLIT("sendToBack")
#define X_ATTRIB								CONSTLIT("x")
#define Y_ATTRIB								CONSTLIT("y")

const int DEFAULT_PORT_POS_RADIUS =				64;
const int DEFAULT_DOCK_DISTANCE_LS =			12;

ALERROR CDockingPortDesc::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pElement, int iScale)

//	InitPortsFromXML
//
//	Initialize ports from an XML element. NOTE: We guarantee that both the polar 
//	and Cartessian port positions are initialized, if required.
//
//	m_iLastRotation is initialized to the rotation of the owner.

	{
	int i;
	int iPortCount;

	//	See if we've got a special element with docking port geometry

	CXMLElement *pDockingPorts = pElement->GetContentElementByTag(DOCKING_PORTS_TAG);
	if (pDockingPorts)
		{
		//	Initialize max dist
		//	NOTE: pOwner can be NULL because sometimes we init ports from a ship class 
		//	(without an object).

		int iDefaultDist = (DEFAULT_DOCK_DISTANCE_LS, (pOwner ? 8 + (int)((pOwner->GetBoundsRadius() / LIGHT_SECOND) + 0.5) : 0));
		m_iMaxDist = pDockingPorts->GetAttributeIntegerBounded(MAX_DIST_ATTRIB, 1, -1, DEFAULT_DOCK_DISTANCE_LS);

		//	Sometimes we specify x,y coordinate but want to convert to rotating 
		//	polar coordinate.

		int iConvertRotation = -1;
		CString sAttrib;
		if (pDockingPorts->FindAttribute(ROTATION_ATTRIB, &sAttrib))
			{
			if (CXMLElement::IsBoolTrueValue(sAttrib))
				iConvertRotation = 0;
			else
				iConvertRotation = strToInt(sAttrib, 0);
			}

		//	If we have sub-elements then these are port definitions.

		iPortCount = pDockingPorts->GetContentElementCount();
		if (iPortCount > 0)
			{
			m_Ports.DeleteAll();
			m_Ports.InsertEmpty(iPortCount);

			for (i = 0; i < iPortCount; i++)
				{
				CXMLElement *pPort = pDockingPorts->GetContentElement(i);

				//	See if the port position is specified with polar coordinate.
				//	If NOT, then we load Cartessian coordinates.

				if (!m_Ports[i].Pos.InitFromXML(pPort, C3DObjectPos::FLAG_NO_XY))
					{
					CVector vDockPos((pPort->GetAttributeInteger(X_ATTRIB) * g_KlicksPerPixel),
							(pPort->GetAttributeInteger(Y_ATTRIB) * g_KlicksPerPixel));

					m_Ports[i].vPos = vDockPos;

					//	If we expect this to be rotatable, then we reverse engineer
					//	the polar coordinates.

					if (iConvertRotation != -1)
						{
						if (iScale <= 0)
							{
							ASSERT(false);
							return ERR_FAIL;
							}

						m_Ports[i].Pos.InitFromXY(iScale, vDockPos, pPort->GetAttributeInteger(POS_Z_ATTRIB));

						//	If the conversion rotation is non-zero then it means that the port
						//	x,y coordinates are for the given rotation, and thus we need to 
						//	compensate.

						if (iConvertRotation != 0)
							m_Ports[i].Pos.SetAngle(m_Ports[i].Pos.GetAngle() - iConvertRotation);
						}
					}

				//	Get the ship's rotation when docked at this port.

				if (pPort->FindAttributeInteger(ROTATION_ATTRIB, &m_Ports[i].iRotation))
					m_Ports[i].iRotation = AngleMod(m_Ports[i].iRotation - (iConvertRotation != -1 ? iConvertRotation : 0));
				else if (!m_Ports[i].Pos.IsEmpty())
					m_Ports[i].iRotation = AngleMod(m_Ports[i].Pos.GetAngle() + 180);
				else
					m_Ports[i].iRotation = AngleMod(VectorToPolar(m_Ports[i].vPos) + 180);

				//	Layer options

				if (pPort->GetAttributeBool(BRING_TO_FRONT_ATTRIB))
					m_Ports[i].iLayer = plBringToFront;
				else if (pPort->GetAttributeBool(SEND_TO_BACK_ATTRIB))
					m_Ports[i].iLayer = plSendToBack;
				}
			}

		//	Otherwise, we expect a port count and radius

		else if ((iPortCount = pDockingPorts->GetAttributeIntegerBounded(PORT_COUNT_ATTRIB, 0, -1, 0)) > 0)
			{
			int iRadius = pDockingPorts->GetAttributeIntegerBounded(PORT_RADIUS_ATTRIB, 0, -1, DEFAULT_PORT_POS_RADIUS);
			int iStartAngle = AngleMod(pDockingPorts->GetAttributeInteger(PORT_ANGLE_ATTRIB));
			int iAngle = 360 / iPortCount;

			//	Initialize ports

			m_Ports.DeleteAll();
			m_Ports.InsertEmpty(iPortCount);

			for (i = 0; i < iPortCount; i++)
				{
				int iPortAngle = AngleMod(iStartAngle + i * iAngle);
				m_Ports[i].Pos = C3DObjectPos(iPortAngle, iRadius);
				m_Ports[i].iRotation = AngleMod(iPortAngle + 180);
				}
			}

		//	Otherwise, no ports

		else
			{
			m_Ports.DeleteAll();
			}
		}

	//	Otherwise, initialize ports based on a count

	else if ((iPortCount = pElement->GetAttributeIntegerBounded(DOCKING_PORTS_ATTRIB, 0, -1, 0)) > 0)
		{
		m_iMaxDist = DEFAULT_DOCK_DISTANCE_LS;

		//	Initialize ports

		int iAngle = 360 / iPortCount;
		Metric rRadius = 64 * g_KlicksPerPixel;

		m_Ports.DeleteAll();
		m_Ports.InsertEmpty(iPortCount);

		for (i = 0; i < iPortCount; i++)
			{
			int iPortAngle = i * iAngle;
			m_Ports[i].Pos = C3DObjectPos(iPortAngle, DEFAULT_PORT_POS_RADIUS);
			m_Ports[i].iRotation = AngleMod(iPortAngle + 180);
			}
		}

	//	Otherwise, no ports

	else
		{
		m_Ports.DeleteAll();
		m_iMaxDist = 0;
		}

	return NOERROR;
	}
