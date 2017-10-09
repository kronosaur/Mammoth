//	TSE.h
//
//	Transcendence Space Engine
//	Copyright 2003 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

#include "Euclid.h"
#include "Lingua.h"
#include "Internets.h"
#include "Crypto.h"
#include "JPEGUtil.h"
#include "XMLUtil.h"
#include "JSONUtil.h"
#include "DirectXUtil.h"
#include "Reanimator.h"

//  Starting with Part II we use Arabic numerals for levels.

//#define LEVEL_ROMAN_NUMERALS

//	Define some debugging symbols

#ifdef DEBUG
//#define DEBUG_ALL_ITEMS
//#define DEBUG_ALL_NODES
//#define DEBUG_ASTAR_PATH
//#define DEBUG_ASTAR_PERF
//#define DEBUG_ATTACK_TARGET
//#define DEBUG_ATTACK_TARGET_MANEUVERS
//#define DEBUG_AVOID_POTENTIAL
//#define DEBUG_BOUNDING_RECT
//#define DEBUG_COMBAT
//#define DEBUG_COMBAT_AI
//#define DEBUG_DOCK_PORT_POS
//#define DEBUG_ENCOUNTER_COUNTS
//#define DEBUG_FIRE_ON_OPPORTUNITY
//#define DEBUG_FORMATION
//#define DEBUG_HENCHMAN
//#define DEBUG_HIT_SCAN
//#define DEBUG_LOAD
//#define DEBUG_NAV_PATH
//#define DEBUG_NEBULA_PAINTING
//#define DEBUG_PARTICLE_BOUNDS
//#define DEBUG_PERFORMANCE
//#define DEBUG_PROGRAM_UPGRADE
//#define DEBUG_RANDOM_SEED
//#define DEBUG_SHIP
//#define DEBUG_SINGLETON_EFFECTS
//#define DEBUG_SOUNDTRACK
//#define DEBUG_SOUNDTRACK_STATE
//#define DEBUG_SOURCE_LOAD_TRACE
//#define DEBUG_STATION_EXCLUSION_ZONE
//#define DEBUG_TARGET_LIST
#define DEBUG_VECTOR
//#define DEBUG_WEAPON_POS
#endif

//	We leave this defined because we want to get traces in the field in case
//	of a crash.
#define DEBUG_PROGRAMSTATE

//	Debugging help

#define DEBUG_TRY					try {
#define DEBUG_CATCH					} catch (...) { kernelDebugLogPattern("Crash in %s", CString(__FUNCTION__)); throw; }
#define DEBUG_CATCH_CONTINUE		} catch (...) { kernelDebugLogPattern("Crash in %s", CString(__FUNCTION__)); }
#define DEBUG_CATCH_MSG(msg)		} catch (...) { kernelDebugLogPattern((msg)); throw; }
#define DEBUG_CATCH_MSG1(msg,p1)	} catch (...) { kernelDebugLogPattern((msg),(p1)); throw; }
#define DEBUG_CATCH_OBJ(pObj)		} catch (...) \
		{ \
		kernelDebugLogPattern("Crash in %s", CString(__FUNCTION__)); \
		\
		if ((pObj) == NULL) \
			kernelDebugLogPattern("pObj = NULL"); \
		else \
			{ \
			CString sError; \
			ReportCrashObj(&sError, (pObj)); \
			kernelDebugLogPattern("pObj = "); \
			kernelDebugLogString(sError); \
			} \
		\
		throw; \
		}

#define DEBUG_TRY_OBJ_LOOP		CSpaceObject *pObj = NULL; try {
#define DEBUG_CATCH_OBJ_LOOP	} catch (...) \
		{ \
		kernelDebugLogPattern("Crash in %s", CString(__FUNCTION__)); \
		\
		if (pObj == NULL) \
			kernelDebugLogPattern("pObj = NULL"); \
		else \
			{ \
			CString sError; \
			ReportCrashObj(&sError, pObj); \
			kernelDebugLogPattern("pObj = "); \
			kernelDebugLogString(sError); \
			} \
		\
		throw; \
		}

//	If ITEM_REFERENCE is defined, then the player doesn't see the
//	stats for an item until they install it (or get reference info)
//#define ITEM_REFERENCE

//	Forward references

class CArmorClass;
class CBoundaryMarker;
class CDeviceClass;
class CDockScreenType;
class CEffectCreator;
class COverlay;
class COverlayType;
class CInstalledDevice;
class CItem;
class CItemListManipulator;
class CItemTable;
class CItemType;
class CMarker;
class CMissile;
class CMission;
class CParticleEffect;
class CPower;
class CRandomEntryResults;
class CResourceDb;
class CShieldClass;
class CShip;
class CShipClass;
class CShipTable;
class CSovereign;
class CSpaceEnvironmentType;
class CSpaceObject;
class CStation;
class CStationType;
class CSystem;
class CSystemEvent;
class CTopologyNode;
class CTradingDesc;
class CUniverse;
class CVector;
class CWeaponClass;
class CWeaponFireDesc;
class IEffectPainter;
class IShipController;
class IShipGenerator;
struct SLocationCriteria;

extern CUniverse *g_pUniverse;

//	Include other TSE files

#include "TSEVersions.h"
#include "TSEGeometry.h"
#include "TSETypes.h"
#include "TSELanguage.h"
#include "TSEUtil.h"
#include "TSETransLisp.h"
#include "TSEDesign.h"
#include "TSEStorage.h"
#include "TSEMultiverse.h"
#include "TSEPaintUtil.h"
#include "TSESystem.h"

//	CResourceDb

class CResourceDb
	{
	public:
		CResourceDb (const CString &sDefFilespec, bool bExtension = false);
		~CResourceDb (void);

		void ComputeFileDigest (CIntegerIP *retDigest);
		IXMLParserController *GetEntities (void) const { return m_pEntities; }
		IXMLParserController *GetEntitiesHandoff (bool *retbFree) { IXMLParserController *pEntities = m_pEntities; *retbFree = m_bFreeEntities; m_pEntities = NULL; m_bFreeEntities = false; return pEntities; }
		const CString &GetFilespec (void) const { return m_sFilespec; }
		CString GetRootTag (void);
		bool ImageExists (const CString &sFolder, const CString &sFilename);
		inline bool IsTDB (void) const { return (m_pDb != NULL); }
		bool IsUsingExternalGameFile (void) const { return !m_bGameFileInDb; }
		bool IsUsingExternalResources (void) const { return !m_bResourcesInDb; }
		ALERROR LoadEmbeddedGameFile (const CString &sFilename, CXMLElement **retpData, IXMLParserController *pResolver, CExternalEntityTable *ioEntityTable, CString *retsError);
		ALERROR LoadEntities (CString *retsError, CExternalEntityTable **retEntities = NULL);
		ALERROR LoadGameFile (CXMLElement **retpData, IXMLParserController *pEntities, CString *retsError, CExternalEntityTable *ioEntityTable = NULL);
		ALERROR LoadGameFileStub (CXMLElement **retpData, CExternalEntityTable *ioEntityTable, CString *retsError);
		ALERROR LoadImage (const CString &sFolder, const CString &sFilename, HBITMAP *rethImage, EBitmapTypes *retiImageType = NULL);
		ALERROR LoadModule (const CString &sFolder, const CString &sFilename, CXMLElement **retpData, CString *retsError);
		ALERROR LoadModuleEntities (const CString &sFolder, const CString &sFilename, CExternalEntityTable **retpEntities, CString *retsError);
		ALERROR LoadSound (CSoundMgr &SoundMgr, const CString &sFolder, const CString &sFilename, int *retiChannel);
		ALERROR Open (DWORD dwFlags, CString *retsError);
		CString ResolveFilespec (const CString &sFolder, const CString &sFilename) const;
		inline void SetDebugMode (bool bValue) { m_bDebugMode = bValue; }
		void SetEntities (IXMLParserController *pEntities, bool bFree = false);

		CString GetResourceFilespec (int iIndex);
		int GetResourceCount (void);
		inline int GetVersion (void) { return m_iVersion; }
		ALERROR ExtractMain (CString *retsData);
		ALERROR ExtractResource (const CString sFilespec, CString *retsData);

	private:
		enum EFlags
			{
			//	SResourceEntry flags
			FLAG_COMPRESS_ZLIB =		0x00000001,
			};

		struct SResourceEntry
			{
			CString sFilename;
			int iEntryID;
			DWORD dwFlags;
			};

		ALERROR OpenDb (void);
		ALERROR ReadEntry (const CString &sFilespec, CString *retsData);

		int m_iVersion;
		bool m_bGameFileInDb;
		bool m_bResourcesInDb;
		bool m_bDebugMode;

		//	If we're just using the file system
		CString m_sFilespec;
		CString m_sRoot;
		CString m_sGameFile;

		//	If we're using a TDB
		CDataFile *m_pDb;
		TSortMap<CString, SResourceEntry> m_ResourceMap;
		int m_iGameFile;

		IXMLParserController *m_pEntities;			//	Entities to use in parsing
		bool m_bFreeEntities;						//	If TRUE, we own m_pEntities;
	};

class CAStarPathFinder
	{
	public:
		CAStarPathFinder (void);
		~CAStarPathFinder (void);

		void AddObstacle (const CVector &vUR, const CVector &vLL);
		int FindPath (const CVector &vStart, const CVector &vEnd, CVector **retPathList, bool bTryReverse = true);

	private:
		struct SObstacle
			{
			CVector vUR;
			CVector vLL;
			};

		struct SNode
			{
			int x;
			int y;

			CVector vPos;
			int iCostFromStart;
			int iHeuristic;
			int iTotalCost;

			SNode *pParent;
			SNode *pPrev;
			SNode *pNext;
			};

		typedef SNode *SNodeRoot;

		void AddToClosedList (SNode *pNew);
		void AddToOpenList (SNode *pNew);
		int CalcHeuristic (const CVector &vPos, const CVector &vDest);
		void CollapsePath (TArray<SNode *> &Path, int iStart, int iEnd);
		void CreateInOpenList (const CVector &vEnd, SNode *pCurrent, int xDir, int yDir);
		bool IsInClosedList (int x, int y);
		bool IsPathClear (const CVector &vStart, const CVector &vEnd);
		bool IsPointClear (const CVector &vPos);
		bool LineIntersectsRect (const CVector &vStart, const CVector &vEnd, const CVector &vUR, const CVector &vLL);
		int OptimizePath (const CVector &vEnd, SNode *pFinal, CVector **retPathList);
		void Reset (void);

		void AddToList (SNodeRoot &pRoot, SNode *pNew, SNode *pAddAfter = NULL);
		void DeleteFromList (SNodeRoot &pRoot, SNode *pDelete);
		void DeleteList (SNodeRoot &pRoot);

		TArray<SObstacle> m_Obstacles;
		SNodeRoot m_pOpenList;
		SNodeRoot m_pClosedList;
		CTileMap *m_pClosedMap;

#ifdef DEBUG_ASTAR_PERF
		int m_iCallsToIsPathClear;
		int m_iClosedListCount;
		int m_iOpenListCount;
#endif
	};

//	Paint Utilities

void ComputeLightningPoints (int iCount, CVector *pPoints, Metric rChaos);
void CreateBlasterShape (int iAngle, int iLength, int iWidth, SPoint *Poly);
void DrawItemTypeIcon (CG32bitImage &Dest, int x, int y, CItemType *pType, int cxSize = 0, int cySize = 0, bool bGray = false);
void DrawLightning (CG32bitImage &Dest,
					int xFrom, int yFrom,
					int xTo, int yTo,
					CG32bitPixel rgbColor,
					int iPoints,
					Metric rChaos);
void DrawLightning (CG32bitImage &Dest,
					int xFrom, int yFrom,
					int xTo, int yTo,
					CG32bitPixel rgbFrom,
					CG32bitPixel rgbTo,
					Metric rChaos);
void DrawParticle (CG32bitImage &Dest,
				   int x, int y,
				   CG32bitPixel rgbColor,
				   int iSize,
				   DWORD byOpacity);

//	Helper functions

CString ComposeDamageAdjReference (int *AdjRow, int *StdRow);
void ComputePercentages (int iCount, int *pTable);

CString ReactorPower2String (int iPower);

int NLCompare (TArray<CString> &Input, TArray<CString> &Pattern);
void NLSplit (const CString &sPhrase, TArray<CString> *retArray);

void ReportCrashObj (CString *retsMessage, CSpaceObject *pCrashObj = NULL);


//	Miscellaneous Structures & Classes

class CShockwaveHitTest
	{
	public:
		void Init (int iSegments, int iLives);
		inline bool IsEmpty (void) const { return (m_Segments.GetCount() == 0); }
		void ReadFromStream (SLoadCtx &Ctx);
		void Update (SEffectUpdateCtx &Ctx, const CVector &vPos, Metric rMinRadius, Metric rMaxRadius);
		void WriteToStream (IWriteStream *pStream) const;

	private:
		void CalcObjBounds (CSpaceObject *pObj, const CVector &vPos, Metric *retrStartAngle, Metric *retrSizeAngle, Metric *retrStartRadius, Metric *retrEndRadius);

		TArray<int> m_Segments;
	};

class CHitCtx
	{
	public:
		CHitCtx (CSpaceObject *pHit = NULL, const CVector &vHitPos = NullVector, int iHitDir = -1) :
				m_pHit(pHit),
				m_vHitPos(vHitPos),
				m_iHitDir(iHitDir)
			{ }

		inline int GetHitDir (void) const { return m_iHitDir; }
		inline CSpaceObject *GetHitObj (void) const { return m_pHit; }
		inline const CVector &GetHitPos (void) const { return m_vHitPos; }

		void ReadFromStream (SLoadCtx &Ctx);
		void WriteToStream (CSystem *pSystem, IWriteStream *pStream) const;

	private:
		CSpaceObject *m_pHit;
		CVector m_vHitPos;
		int m_iHitDir;
	};

//	Topology

class CTopology
	{
	public:
		CTopology (void);
		~CTopology (void);

		ALERROR AddStargateFromXML (STopologyCreateCtx &Ctx, CXMLElement *pDesc, CTopologyNode *pNode = NULL, bool bRootNode = false);
		ALERROR AddTopology (STopologyCreateCtx &Ctx);
		ALERROR AddTopologyDesc (STopologyCreateCtx &Ctx, CTopologyDesc *pNode, CTopologyNode **retpNewNode = NULL);
		ALERROR AddTopologyNode (STopologyCreateCtx &Ctx, const CString &sNodeID, CTopologyNode **retpNewNode = NULL);
		void DeleteAll (void);
		CTopologyNode *FindTopologyNode (const CString &sID);
		int GetDistance (const CString &sSourceID, const CString &sDestID);
		inline CTopologyNodeList &GetTopologyNodeList (void) { return m_Topology; }
		inline CTopologyNode *GetTopologyNode (int iIndex) { return m_Topology.GetAt(iIndex); }
		inline int GetTopologyNodeCount (void) { return m_Topology.GetCount(); }
		ALERROR InitComplexArea (CXMLElement *pAreaDef, int iMinRadius, CComplexArea *retArea, STopologyCreateCtx *pCtx = NULL, CTopologyNode **iopExit = NULL); 
		void ReadFromStream (SUniverseLoadCtx &Ctx);

	private:
		enum NodeTypes
			{
			typeStandard,
			typeFragmentStart,
			typeFragment,
			};

		ALERROR AddFragment (STopologyCreateCtx &Ctx, CTopologyDesc *pFragment, CTopologyNode **retpNewNode);
		ALERROR AddNetwork (STopologyCreateCtx &Ctx, CTopologyDesc *pNetwork, CTopologyNode **retpNewNode);
		ALERROR AddNode (STopologyCreateCtx &Ctx, CTopologyDesc *pNode, CTopologyNode **retpNewNode);
		ALERROR AddNodeGroup (STopologyCreateCtx &Ctx, CTopologyDesc *pTable, CTopologyNode **retpNewNode);
		ALERROR AddNodeTable (STopologyCreateCtx &Ctx, CTopologyDesc *pTable, CTopologyNode **retpNewNode);

		ALERROR AddRandom (STopologyCreateCtx &Ctx, CTopologyDesc *pDesc, CTopologyNode **retpNewNode);
		ALERROR AddRandomParsePosition (STopologyCreateCtx *pCtx, const CString &sValue, CTopologyNode **iopExit, int *retx, int *rety);
		ALERROR AddRandomRegion (STopologyCreateCtx &Ctx, CTopologyDesc *pDesc, CXMLElement *pRegionDef, CTopologyNode *&pExitNode, CIntGraph &Graph, TArray<CTopologyNode *> &Nodes);

		ALERROR AddStargate (STopologyCreateCtx &Ctx, CTopologyNode *pNode, bool bRootNode, CXMLElement *pGateDesc);
		ALERROR AddTopologyNode (STopologyCreateCtx &Ctx,
								 CSystemMap *pMap,
								 const CString &sID,
								 bool bNoMap,
								 int x,
								 int y,
								 const CString &sAttribs,
								 CXMLElement *pSystemDesc,
								 CEffectCreator *pEffect,
								 CTopologyNode **retpNode = NULL);
		ALERROR AddTopologyNode (const CString &sID, CTopologyNode *pNode);
		CString ExpandNodeID (STopologyCreateCtx &Ctx, const CString &sID);
		ALERROR FindTopologyDesc (STopologyCreateCtx &Ctx, const CString &sNodeID, CTopologyDesc **retpNode, NodeTypes *retiNodeType = NULL);
		CString GenerateUniquePrefix (const CString &sPrefix, const CString &sTestNodeID);
		void GetAbsoluteDisplayPos (STopologyCreateCtx &Ctx, int x, int y, int *retx, int *rety, int *retiRotation);
		int GetDistance (CTopologyNode *pSource, const CString &sDestID, int iBestDist = -1);
		void GetFragmentDisplayPos (STopologyCreateCtx &Ctx, CTopologyNode *pNode, int *retx, int *rety);
		ALERROR GetOrAddTopologyNode (STopologyCreateCtx &Ctx, const CString &sID, CTopologyNode *pPrevNode, CXMLElement *pGateDesc, CTopologyNode **retpNode);

		CTopologyNodeList m_Topology;
		TSortMap<CString, int> m_IDToNode;
	};

//	Attack Detector

class CAttackDetector
	{
	public:
		CAttackDetector (void);

		inline void Blacklist (void) { m_iCounter = -1; }
		inline void ClearBlacklist (void) { m_iCounter = 0; }
		inline bool IsBlacklisted (void) { return m_iCounter == -1; }
		bool Hit (int iTick);
		void ReadFromStream (SLoadCtx &Ctx);
		inline void Update (int iTick) { if ((iTick % DECAY_RATE) == 0) OnUpdate(); }
		void WriteToStream (IWriteStream *pStream);

	private:
		enum Constants
			{
			DECAY_RATE = 150,
			HIT_LIMIT = 3,
			MULTI_HIT_WINDOW = 20,
			};

		void OnUpdate (void);

		int m_iCounter;
		int m_iLastHit;
	};

//	Item Event Handler

enum EItemEventDispatchTypes
	{
	dispatchFireEvent,
	dispatchCheckEnhancementLifetime,
	};

class CItemEventDispatcher
	{
	public:
		CItemEventDispatcher (void);
		~CItemEventDispatcher (void);

		inline void FireEvent (CSpaceObject *pSource, ECodeChainEvents iEvent)	{ if (m_pFirstEntry) FireEventFull(pSource, iEvent); }
		inline void FireUpdateEvents (CSpaceObject *pSource) { if (m_pFirstEntry) FireUpdateEventsFull(pSource); }
		void Init (CSpaceObject *pSource);

	private:
		struct SEntry
			{
			EItemEventDispatchTypes iType;				//	Type of entry
			CItem theItem;								//	Item

			ECodeChainEvents iEvent;					//	Event (if dispatchFireEvent)
			SEventHandlerDesc Event;					//	Code (if dispatchFireEvent)

			DWORD dwEnhancementID;						//	ID of enhancement (if relevant)

			SEntry *pNext;
			};

		void AddEntry (const CString &sEvent, EItemEventDispatchTypes iType, const SEventHandlerDesc &Event, const CItem &Item, DWORD dwEnhancementID);
		SEntry *AddEntry (void);
		void FireEventFull (CSpaceObject *pSource, ECodeChainEvents iEvent);
		void FireUpdateEventsFull (CSpaceObject *pSource);
		void RemoveAll (void);

		SEntry *m_pFirstEntry;
	};

//	Ship classes and types

class COverlay
	{
	public:
		COverlay (void);
		~COverlay (void);
		static void CreateFromType (COverlayType *pType, 
									int iPosAngle,
									int iPosRadius,
									int iRotation,
									int iLifeLeft, 
									COverlay **retpField);

		bool AbsorbDamage (CSpaceObject *pSource, SDamageCtx &Ctx);
		void AccumulateBounds (CSpaceObject *pSource, int iScale, int iRotation, RECT *ioBounds);
		void Destroy (CSpaceObject *pSource);
		inline bool Disarms (CSpaceObject *pSource) const { return m_pType->Disarms(); }
		void FireCustomEvent (CSpaceObject *pSource, const CString &sEvent, ICCItem *pData, ICCItem **retpResult);
		bool FireGetDockScreen (CSpaceObject *pSource, CString *retsScreen, int *retiPriority, ICCItem **retpData) const;
		void FireOnCreate (CSpaceObject *pSource);
		bool FireOnDamage (CSpaceObject *pSource, SDamageCtx &Ctx);
		void FireOnDestroy (CSpaceObject *pSource);
		void FireOnObjDestroyed (CSpaceObject *pSource, const SDestroyCtx &Ctx) const;
		void FireOnObjDocked (CSpaceObject *pSource, CSpaceObject *pShip) const;
		inline int GetCounter (void) const { return m_iCounter; }
		inline const CString &GetData (const CString &sAttrib) { return m_Data.GetData(sAttrib); }
		inline int GetDevice (void) const { return m_iDevice; }
		inline Metric GetDrag (CSpaceObject *pSource) const { return m_pType->GetDrag(); }
		inline DWORD GetID (void) const { return m_dwID; }
		inline const CString &GetMessage (void) const { return m_sMessage; }
		inline COverlay *GetNext (void) const { return m_pNext; }
		CVector GetPos (CSpaceObject *pSource);
		ICCItem *GetProperty (CCodeChainCtx *pCCCtx, CSpaceObject *pSource, const CString &sName);
		inline int GetRotation (void) const { return m_iRotation; }
		inline COverlayType *GetType(void) const { return m_pType; }
        inline void IncData (const CString &sAttrib, ICCItem *pValue = NULL, ICCItem **retpNewValue = NULL) { m_Data.IncData(sAttrib, pValue, retpNewValue); }
		inline bool IsDestroyed (void) const { return (m_fDestroyed ? true : false); }
		inline bool IsFading (void) const { return (m_fFading ? true : false); }
		inline bool IsShieldOverlay (void) const { return m_pType->IsShieldOverlay(); }
		inline bool IsShipScreenDisabled (void) const { return m_pType->IsShipScreenDisabled(); }
		void Paint (CG32bitImage &Dest, int iScale, int x, int y, SViewportPaintCtx &Ctx);
		void PaintAnnotations (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		void PaintBackground (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		void PaintLRSAnnotations (const ViewportTransform &Trans, CG32bitImage &Dest, int x, int y);
		void PaintMapAnnotations (CMapViewportCtx &Ctx, CG32bitImage &Dest, int x, int y);
		inline bool Paralyzes (CSpaceObject *pSource) const { return m_pType->Paralyzes(); }
		void ReadFromStream (SLoadCtx &Ctx);
		inline void SetData (const CString &sAttrib, const CString &sData) { m_Data.SetData(sAttrib, sData); }
		inline void SetDevice (int iDev) { m_iDevice = iDev; }
		bool SetEffectProperty (const CString &sProperty, ICCItem *pValue);
		inline void SetNext (COverlay *pNext) { m_pNext = pNext; }
		void SetPos (CSpaceObject *pSource, const CVector &vPos);
		bool SetProperty (CSpaceObject *pSource, const CString &sName, ICCItem *pValue);
		inline void SetRotation (int iRotation) { m_iRotation = iRotation; }
		inline bool Spins (CSpaceObject *pSource) const { return m_pType->Spins(); }
		void Update (CSpaceObject *pSource, int iScale, int iRotation, bool *retbModified = NULL);
		void WriteToStream (IWriteStream *pStream);

	private:
		void CalcOffset (int iScale, int iRotation, int *retxOffset, int *retyOffset, int *retiRotationOrigin = NULL) const;
		void FireOnUpdate (CSpaceObject *pSource);
		void CreateHitEffect (CSpaceObject *pSource, SDamageCtx &Ctx);

		COverlayType *m_pType;					//	Type of field
		DWORD m_dwID;							//	Universal ID
		int m_iTick;							//	Overlay tick
		int m_iLifeLeft;						//	Ticks left of energy field life (-1 = permanent)
		int m_iDevice;							//	Index of device that we're associated with (-1 if not a device)

		int m_iPosAngle;						//	Position relative to source (degrees)
		int m_iPosRadius;						//	Position relative to source (pixels)
		int m_iRotation;						//	Overlay orientation (degrees)

		CAttributeDataBlock m_Data;				//	data
		int m_iCounter;							//	Arbitrary counter
		CString m_sMessage;						//	Message text

		IEffectPainter *m_pPainter;				//	Painter

		int m_iPaintHit;						//	If >0 then we paint a hit
		int m_iPaintHitTick;					//	Tick for hit painter
		IEffectPainter *m_pHitPainter;			//	Hit painter

		DWORD m_fDestroyed:1;					//	TRUE if field should be destroyed
		DWORD m_fFading:1;						//	TRUE if we're destroyed, but fading the effect

		COverlay *m_pNext;					//	Next energy field associated with this object
	};

class COverlayList
	{
	public:
		struct SImpactDesc
			{
			SImpactDesc (void) :
					bDisarm(false),
					bParalyze(false),
					bSpin(false),
					bShipScreenDisabled(false),
					rDrag(1.0)
				{ }

			bool bDisarm;					//	TRUE if source is disarmed
			bool bParalyze;					//	TRUE if source is paralyzed.
			bool bSpin;						//	TRUE if source should spin wildly.
			bool bShipScreenDisabled;		//	TRUE if source cannot bring up ship screen.

			Metric rDrag;					//	Drag coefficient (1.0 = no drag)
			};

		COverlayList (void);
		~COverlayList (void);

		void AddField (CSpaceObject *pSource, 
					   COverlayType *pType,
					   int iPosAngle,
					   int iPosRadius,
					   int iRotation,
					   int iLifeLeft, 
					   DWORD *retdwID = NULL);
		bool AbsorbDamage (CSpaceObject *pSource, SDamageCtx &Ctx);
		bool AbsorbsWeaponFire (CInstalledDevice *pDevice);
		void AccumulateBounds (CSpaceObject *pSource, int iScale, int iRotation, RECT *ioBounds);
		bool Damage (CSpaceObject *pSource, SDamageCtx &Ctx);
		CString DebugCrashInfo (void) const;
		bool FireGetDockScreen (CSpaceObject *pSource, CString *retsScreen = NULL, int *retiPriority = NULL, ICCItem **retpData = NULL) const;
		void FireOnObjDestroyed (CSpaceObject *pSource, const SDestroyCtx &Ctx) const;
		void FireOnObjDocked (CSpaceObject *pSource, CSpaceObject *pShip) const;
		int GetCountOfType (COverlayType *pType);
		const CString &GetData (DWORD dwID, const CString &sAttrib);
		void GetImpact (CSpaceObject *pSource, SImpactDesc *retImpact) const;
		void GetList (TArray<COverlay *> *retList);
		void GetListOfCommandPaneCounters (TArray<COverlay *> *retList);
		COverlay *GetOverlay (DWORD dwID) const;
		CVector GetPos (CSpaceObject *pSource, DWORD dwID);
		ICCItem *GetProperty (CCodeChainCtx *pCCCtx, CSpaceObject *pSource, DWORD dwID, const CString &sName);
		int GetRotation (DWORD dwID);
		COverlayType *GetType(DWORD dwID);
		int GetWeaponBonus (CInstalledDevice *pDevice, CSpaceObject *pSource);
        void IncData (DWORD dwID, const CString &sAttrib, ICCItem *pValue = NULL, ICCItem **retpNewValue = NULL);
		inline bool IsEmpty (void) { return (m_pFirst == NULL); }
		void Paint (CG32bitImage &Dest, int iScale, int x, int y, SViewportPaintCtx &Ctx);
		void PaintAnnotations (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		void PaintBackground (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		void PaintLRSAnnotations (const ViewportTransform &Trans, CG32bitImage &Dest, int x, int y);
		void PaintMapAnnotations (CMapViewportCtx &Ctx, CG32bitImage &Dest, int x, int y);
		void ReadFromStream (SLoadCtx &Ctx, CSpaceObject *pSource);
		void RemoveField (CSpaceObject *pSource, DWORD dwID);
		void ScrapeHarmfulOverlays (CSpaceObject *pSource, int iMaxRemoved = 1);
		void SetData (DWORD dwID, const CString &sAttrib, const CString &sData);
		bool SetEffectProperty (DWORD dwID, const CString &sProperty, ICCItem *pValue);
		void SetPos (CSpaceObject *pSource, DWORD dwID, const CVector &vPos);
		bool SetProperty (CSpaceObject *pSource, DWORD dwID, const CString &sName, ICCItem *pValue);
		void SetRotation (DWORD dwID, int iRotation);
		void Update (CSpaceObject *pSource, int iScale, int iRotation, bool *retbModified);
		void WriteToStream (IWriteStream *pStream);

	private:
		COverlay *m_pFirst;
	};

//	Space Objects

#define MAX_COMMS_CAPS					25

//	CSpaceObject ---------------------------------------------------------------
//
//	SEMANTICS
//
//	CanAttack: If TRUE, this is an active/intelligent object that could pose
//		a threat. E.g., a ship or a station. Generally used to pick out a 
//		possible target. Missiles/beams cannot attack. Wrecks and abandoned
//		stations cannot attack.
//
//	Hidden: Hidden objects are never shown in the system. For example, suspended
//		objects are hidden. Anything that paints, e.g., fading missiles, is not
//		hidden.
//
//	Inactive: This is almost identical to !CanAttack, except that missiles are
//		considered active (unless they are fading out).
//
//	Intangible: If TRUE, this object is not a physical object in the system.
//		Virtual and hidden objects are intangible. So are missiles fading out.

class CSpaceObject : public CObject
	{
	public:
		enum Categories
			{
			catShip =			0x00000001,
			catStation =		0x00000002,
			catBeam =			0x00000004,
			catMissile =		0x00000008,
			catFractureEffect =	0x00000010,
			catMarker =			0x00000020,
			catOther =			0x80000000,
			};

		enum StealthAndPerceptionLevels
			{
			stealthMin =			0,
			stealthNormal =			4,
			stealthMax =			15,			//	Cloaked

			perceptMin =			0,
			perceptNormal =			4,
			perceptMax =			15,
			};

		enum InterSystemResults
			{
			interNoAction =					0,
			interFollowPlayer =				1,
			interWaitForPlayer =			2,
			};

		//	NOTE: Order must match INSTALL_ITEM_RESULTS_TABLE in CSpaceObject

		enum InstallItemResults
			{
			insOK = 0,							//	OK to install

			insArmorTooHeavy,					//	Armor is too heavy for ship
			insCannotInstall,					//	Cannot install (due to custom reason)
			insNoDeviceSlotsLeft,				//	No device slots left
			insNoNonWeaponSlotsLeft,			//	No non-weapon slots left
			insNoWeaponSlotsLeft,				//	No weapon slots left
			insNotInstallable,					//	Item cannot be installed
			insReactorIncompatible,				//	Reactor is too powerful for ship class
			insReactorOverload,					//	Sum of device power > than reactor output
			insReactorTooWeak,					//	Reactor output too small for device
			insNotCompatible,					//	Item does not match criteria in ship class

			insReplaceCargo,					//	OK, but cargo device will be replaced
			insReplaceDrive,					//	OK, but drive device will be replaced
			insReplaceLauncher,					//	OK, but launcher will be replaced
			insReplaceOther,					//	OK, but another device will be replaced
			insReplaceReactor,					//	OK, but reactor will be replaced
			insReplaceShields,					//	OK, but shields will be replaced

			insInstallItemResultsCount,			//	Must be last enum
			};

		enum CriteriaSortTypes
			{
			sortNone,
			sortByDistance,
			};

		enum CriteriaPosCheckTypes
			{
			checkNone,
			checkPosIntersect,
			checkLineIntersect,
			};

		struct Criteria
			{
			CSpaceObject *pSource;				//	Source

			DWORD dwCategories;					//	Only these object categories
			bool bSelectPlayer;					//	Select the player
			bool bIncludeVirtual;				//	Include virtual objects
			bool bActiveObjectsOnly;			//	Only active object (e.g., objects that can attack)
			bool bKilledObjectsOnly;			//	Only objects that cannot attack
			bool bFriendlyObjectsOnly;			//	Only friendly to source
			bool bEnemyObjectsOnly;				//	Only enemy to source
			bool bAngryObjectsOnly;				//	Only enemy and angry objects
			bool bManufacturedObjectsOnly;		//	Exclude planets, stars, etc.
			bool bStructureScaleOnly;			//	Only structure-scale objects
			bool bStargatesOnly;				//	Only stargates
			bool bNearestOnly;					//	The nearest object to the source
			bool bFarthestOnly;					//	The fartest object to the source
			bool bNearerThan;					//	Only objects nearer than rMinRadius
			bool bFartherThan;					//	Only objects farther than rMaxRadius
			bool bHomeBaseIsSource;				//	Only objects whose home base is the source
			bool bDockedWithSource;				//	Only objects currently docked with source
			bool bExcludePlayer;				//	Exclude the player
			bool bTargetIsSource;				//	Only objects whose target is the source

			bool bPerceivableOnly;				//	Only objects that can be perceived by the source
			int iPerception;					//	Cached perception of pSource

			bool bSourceSovereignOnly;			//	Only objects the same sovereign as source
			DWORD dwSovereignUNID;				//	Only objects with this sovereign UNID

			CString sData;						//	Only objects with non-Nil data
			CString sStargateID;				//	Only objects with this stargate ID (if non blank)
			Metric rMinRadius;					//	Only objects at or beyond the given radius
			Metric rMaxRadius;					//	Only objects within the given radius
			int iIntersectAngle;				//	Only objects that intersect line from source
			IShipController::OrderTypes iOrder;	//	Only objects with this order

			TArray<CString> AttribsRequired;	//	Required attributes
			TArray<CString> AttribsNotAllowed;	//	Exclude objects with these attributes
			TArray<CString> SpecialRequired;	//	Special required attributes
			TArray<CString> SpecialNotAllowed;	//	Special excluding attributes

			int iEqualToLevel;					//	Objects of this level
			int iGreaterThanLevel;
			int iLessThanLevel;

			CriteriaPosCheckTypes iPosCheck;
			CVector vPos1;
			CVector vPos2;

			CriteriaSortTypes iSort;
			ESortOptions iSortOrder;
			};

		struct SCriteriaMatchCtx
			{
			SCriteriaMatchCtx (const Criteria &Crit);

			//	Computed from criteria
			Metric rMinRadius2;
			Metric rMaxRadius2;

			//	Nearest/farthest object
			CSpaceObject *pBestObj;
			Metric rBestDist2;

			//	Sorted results
			TSortMap<Metric, CSpaceObject *> DistSort;

			//	Temporaries
			bool bCalcPolar;
			bool bCalcDist2;
			};

		struct SOnCreate
			{
			SOnCreate (void) :
					pCreateCtx(NULL),
					pData(NULL),
					pBaseObj(NULL),
					pTargetObj(NULL),
					pOwnerObj(NULL),
					pOrbit(NULL)
				{ }

			SSystemCreateCtx *pCreateCtx;
			ICCItem *pData;
			CSpaceObject *pBaseObj;
			CSpaceObject *pTargetObj;
			CSpaceObject *pOwnerObj;
			const COrbit *pOrbit;
			};

		//	Creation and Lifetime

		CSpaceObject (IObjectClass *pClass);
		virtual ~CSpaceObject (void);

		static void CreateFromStream (SLoadCtx &Ctx, CSpaceObject **retpObj);

		ALERROR AddToSystem (CSystem *pSystem, bool bNoGlobalInsert = false);
		void Ascend (void);
		inline void Destroy (SDamageCtx &Ctx) { Destroy(Ctx.Damage.GetCause(), Ctx.Attacker, Ctx.pDesc); }
		inline void Destroy (DestructionTypes iCause, SDamageCtx &Ctx) { Destroy(iCause, Ctx.Attacker, Ctx.pDesc); }
		void Destroy (DestructionTypes iCause, const CDamageSource &Attacker, CWeaponFireDesc *pWeaponDesc = NULL, CSpaceObject **retpWreck = NULL);
		inline CSystem *GetSystem (void) const { return m_pSystem; }
		inline CUniverse *GetUniverse (void) const { return m_pSystem->GetUniverse(); }
		inline bool IsAscended (void) const { return m_fAscended; }
		void Remove (DestructionTypes iCause, const CDamageSource &Attacker, bool bRemovedByOwner = false);
		inline void SetAscended (bool bAscended = true) { m_fAscended = bAscended; }

		//	Basics

		void SetSovereign (CSovereign *pSovereign);

		//	Docking

		virtual CSpaceObject *GetDockedObj (void) const { return NULL; }
		virtual CVector GetDockingPortOffset (int iRotation) { return NullVector; }
		virtual void OnDocked (CSpaceObject *pObj) { }
		virtual void OnDockedObjChanged (CSpaceObject *pLocation) { }

		//	Docking Ports

		virtual void CreateRandomDockedShips (IShipGenerator *pGenerator, const CShipChallengeDesc &ShipsNeeded = CShipChallengeDesc()) { }
		virtual CDockingPorts *GetDockingPorts (void) { return NULL; }
		virtual bool RequestDock (CSpaceObject *pObj, int iPort = -1) { return false; }
		virtual void Undock (CSpaceObject *pObj) { }

		inline int GetDockingPortCount (void) { CDockingPorts *pPorts = GetDockingPorts(); return (pPorts ? pPorts->GetPortCount(this) : 0); }
		int GetNearestDockPort (CSpaceObject *pRequestingObj, CVector *retvPort = NULL);
		inline int GetOpenDockingPortCount (void) { CDockingPorts *pPorts = GetDockingPorts(); return (pPorts ? (pPorts->GetPortCount(this) - pPorts->GetPortsInUseCount(this)) : 0); }
		inline CSpaceObject *GetShipAtDockingPort (int iPort) { CDockingPorts *pPorts = GetDockingPorts(); return (pPorts ? pPorts->GetPortObj(this, iPort) : NULL); }
		inline bool IsObjDocked (CSpaceObject *pObj) { CDockingPorts *pPorts = GetDockingPorts(); return (pPorts ? pPorts->IsObjDocked(pObj) : false); }
		inline bool IsObjDockedOrDocking (CSpaceObject *pObj) { CDockingPorts *pPorts = GetDockingPorts(); return (pPorts ? pPorts->IsObjDockedOrDocking(pObj) : false); }
		inline void PlaceAtRandomDockPort (CSpaceObject *pObj) { CDockingPorts *pPorts = GetDockingPorts(); if (pPorts) pPorts->DockAtRandomPort(this, pObj); }
		bool SupportsDocking (bool bPlayer = false);
		inline bool SupportsDockingFast (void) const { return (m_fHasDockScreenMaybe ? true : false); }

		//	Dock Screens

		virtual DWORD GetDefaultBkgnd (void) { return 0; }
		virtual CDesignType *GetDefaultDockScreen (CString *retsName = NULL) { return NULL; }
		virtual CXMLElement *GetScreen (const CString &sName);

		//	Freeze controls

		inline bool CanBeControlled (void) { return m_iControlsFrozen == 0; }
		inline void FreezeControls (void) { m_iControlsFrozen++; }
		inline void UnfreezeControls (void) { m_iControlsFrozen--; }

		//	Fuel

		virtual void ConsumeFuel (Metric rFuel, CReactorDesc::EFuelUseTypes iUse = CReactorDesc::fuelConsume) { }
		virtual bool IsOutOfPower (void) { return false; }
		virtual void Refuel (Metric Fuel) { }
		virtual void Refuel (const CItem &Fuel) { }

		bool HasFuelItem (void);

		//	Items

		EnhanceItemStatus AddItemEnhancement (const CItem &itemToEnhance, CItemType *pEnhancement, int iLifetime, DWORD *retdwID);
		EnhanceItemStatus AddItemEnhancement (CItemListManipulator &ItemList, CItemType *pEnhancement, int iLifetime, DWORD *retdwID);
		void DamageItem (CInstalledDevice *pDevice);
		void DamageItem (CItemListManipulator &ItemList);
		void DisruptItem (CItemListManipulator &ItemList, DWORD dwDuration);
		EnhanceItemStatus EnhanceItem (CItemListManipulator &ItemList, const CItemEnhancement &Mods, DWORD *retdwID = NULL);
		CItem GetItemForDevice (CInstalledDevice *pDevice);
		inline CItemList &GetItemList (void) { return m_ItemList; }
		ICCItem *GetItemProperty (CCodeChainCtx &CCCtx, const CItem &Item, const CString &sName);
		void RemoveItemEnhancement (const CItem &itemToEnhance, DWORD dwID, bool bExpiredOnly = false);
		void RepairItem (CItemListManipulator &ItemList);
		void SetCursorAtArmor (CItemListManipulator &ItemList, CInstalledArmor *pArmor);
		bool SetCursorAtDevice (CItemListManipulator &ItemList, int iDevSlot);
		bool SetCursorAtDevice (CItemListManipulator &ItemList, CInstalledDevice *pDevice);
		void SetCursorAtRandomItem (CItemListManipulator &ItemList, const CItemCriteria &Crit);
		bool SetItemProperty (const CItem &Item, const CString &sName, ICCItem *pValue, int iCount, CItem *retItem, CString *retsError);
		bool Translate (const CString &sID, ICCItem *pData, CString *retsText);
		bool Translate (const CString &sID, ICCItem *pData, ICCItem **retpResult);
		CString Translate (const CString &sID, ICCItem *pData, const CString &sDefault);
		void UseItem (CItem &Item, CString *retsError = NULL);

		inline void InvalidateItemListAddRemove (void) { m_fItemEventsValid = false; }
		inline void InvalidateItemListState (void) { m_fItemEventsValid = false; }
		void ItemsModified (void);

		//	Joints
		//
		//	NOTE: "Attached" implies a permanent joint as when a ship is 
		//	composed of multiple sections. An object that is attached is NOT an
		//	independent object.

		virtual CSpaceObject *GetAttachedRoot (void) const { return NULL; }
		virtual bool IsAttached (void) const { return false; }

		inline CObjectJoint *GetFirstJoint (void) const { return m_pFirstJoint; }
		inline void SetFirstJoint (CObjectJoint *pJoint) { m_pFirstJoint = pJoint; }

		//	Miscellaneous

		void AddEffect (IEffectPainter *pPainter, const CVector &vPos, int iTick = 0, int iRotation = 0);
		void AddEventSubscriber (CSpaceObject *pObj);
		inline bool Blocks (CSpaceObject *pObj) { return (m_fIsBarrier && CanBlock(pObj)); }
		inline bool BlocksShips (void) { return (m_fIsBarrier && CanBlockShips()); }
		int CalcFireSolution (CSpaceObject *pTarget, Metric rMissileSpeed) const;
		CSpaceObject *CalcTargetToAttack (CSpaceObject *pAttacker, CSpaceObject *pOrderGiver);
		inline bool CanBeHit (void) { return (!m_fCannotBeHit && !m_fOutOfPlaneObj); }
		inline bool CanBeHitByFriends (void) { return !m_fNoFriendlyTarget; }
		bool CanDetect (int Perception, CSpaceObject *pObj);
		bool CanCommunicateWith (CSpaceObject *pSender);
		inline bool CanHitFriends (void) { return !m_fNoFriendlyFire; }
		inline void ClearNoFriendlyTarget (void) { m_fNoFriendlyTarget = false; }
		inline void ClearPaintNeeded (void) { m_fPaintNeeded = false; }
		inline void ClearPlayerDestination (void) { m_fPlayerDestination = false; m_fAutoClearDestination = false; m_fAutoClearDestinationOnDock = false; m_fAutoClearDestinationOnDestroy = false; m_fShowDistanceAndBearing = false; m_fShowHighlight = false; }
		inline void ClearPlayerDocked (void) { m_fPlayerDocked = false; }
		inline void ClearPlayerTarget (void) { m_fPlayerTarget = false; }
		inline void ClearPOVLRS (void) { m_fInPOVLRS = false; }
		inline void ClearSelection (void) { m_fSelected = false; }
		inline void ClearShowDamageBar (void) { m_fShowDamageBar = false; }
		inline void ClearShowDistanceAndBearing (void) { m_fShowDistanceAndBearing = false; }
		void CommsMessageFrom (CSpaceObject *pSender, int iIndex);
		inline DWORD Communicate (CSpaceObject *pReceiver, MessageTypes iMessage, CSpaceObject *pParam1 = NULL, DWORD dwParam2 = 0) { return pReceiver->OnCommunicate(this, iMessage, pParam1, dwParam2); }
		void CopyDataFromObj (CSpaceObject *pSource);
		ALERROR CreateRandomItems (CXMLElement *pItems, CSystem *pSystem);
		ALERROR CreateRandomItems (IItemGenerator *pItems, CSystem *pSystem);
		EDamageResults Damage (SDamageCtx &Ctx);
		static CString DebugDescribe (CSpaceObject *pObj);
		inline bool DebugIsValid (void) { return (DWORD)m_pSystem != 0xdddddddd; }
		static CString DebugLoadError (SLoadCtx &Ctx);
		void EnterGate (CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate, bool bAscend = false);
		int FindCommsMessage (const CString &sID);
		int FindCommsMessageByName (const CString &sName);
		bool FindDevice (const CItem &Item, CInstalledDevice **retpDevice, CString *retsError);
		bool FindEventHandler (const CString &sEntryPoint, SEventHandlerDesc *retEvent = NULL);
		bool FindEventHandler (CDesignType::ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL);
		inline bool FindEventSubscriber (CSpaceObject *pObj) { return m_SubscribedObjs.FindObj(pObj); }
		bool FireCanDockAsPlayer (CSpaceObject *pDockTarget, CString *retsError);
		bool FireCanInstallItem (const CItem &Item, int iSlot, CString *retsResult);
		bool FireCanRemoveItem (const CItem &Item, int iSlot, CString *retsResult);
		void FireCustomEvent (const CString &sEvent, ECodeChainEvents iEvent = eventNone, ICCItem *pData = NULL, ICCItem **retpResult = NULL);
		void FireCustomItemEvent (const CString &sEvent, const CItem &Item, ICCItem *pData, ICCItem **retpResult = NULL);
		void FireCustomOverlayEvent (const CString &sEvent, DWORD dwID, ICCItem *pData, ICCItem **retpResult = NULL);
		void FireCustomShipOrderEvent (const CString &sEvent, CSpaceObject *pShip, ICCItem **retpResult = NULL);
		bool FireGetDockScreen (CString *retsScreen = NULL, int *retiPriority = NULL, ICCItem **retpData = NULL);
		void FireGetExplosionType (SExplosionType *retExplosion);
		bool FireGetPlayerPriceAdj (STradeServiceCtx &ServiceCtx, ICCItem *pData, int *retiPriceAdj);
		void FireItemOnAIUpdate (void);
		void FireItemOnDocked (CSpaceObject *pDockedAt);
		void FireItemOnObjDestroyed (const SDestroyCtx &Ctx);
		void FireItemOnUpdate (void);
		void FireOnAttacked (const SDamageCtx &Ctx);
		void FireOnAttackedByPlayer (void);
		void FireOnCreate (void);
		void FireOnCreate (const SOnCreate &OnCreate);
		void FireOnCreateOrders (CSpaceObject *pBase, CSpaceObject *pTarget);
		void FireOnDamage (SDamageCtx &Ctx);
		void FireOnDeselected (void);
		void FireOnDestroy (const SDestroyCtx &Ctx);
		bool FireOnDockObjAdj (CSpaceObject **retpObj);
		void FireOnEnteredGate (CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pGate);
		void FireOnEnteredSystem (CSpaceObject *pGate);
		void FireOnLoad (SLoadCtx &Ctx);
		void FireOnMining (const SDamageCtx &Ctx);
		void FireOnMissionAccepted (CMission *pMission);
		void FireOnMissionCompleted (CMission *pMission, const CString &sReason);
		void FireOnObjBlacklistedPlayer (CSpaceObject *pObj);
		void FireOnObjDestroyed (const SDestroyCtx &Ctx);
		void FireOnObjDocked (CSpaceObject *pObj, CSpaceObject *pDockTarget);
		void FireOnObjEnteredGate (CSpaceObject *pObj, CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate);
		bool FireOnObjGate (CSpaceObject *pObj);
		void FireOnObjJumped (CSpaceObject *pObj);
		bool FireOnObjJumpPosAdj (CSpaceObject *pObj, CVector *iovPos);
		void FireOnObjReconned (CSpaceObject *pObj);
		void FireOnOrderChanged (void);
		void FireOnOrdersCompleted (void);
		void FireOnPlayerBlacklisted (void);
		void FireOnPlayerEnteredShip (CSpaceObject *pOldShip);
		InterSystemResults FireOnPlayerEnteredSystem (CSpaceObject *pPlayer);
		void FireOnPlayerLeftShip (CSpaceObject *pNewShip);
		InterSystemResults FireOnPlayerLeftSystem (CSpaceObject *pPlayer, CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate);
		void FireOnSelected (void);
		bool FireOnSubordinateAttacked (const SDamageCtx &Ctx);
		void FireOnSystemExplosion (CSpaceObject *pExplosion, CSpaceObject *pSource, DWORD dwItemUNID);
		void FireOnSystemObjAttacked (SDamageCtx &Ctx);
		void FireOnSystemObjDestroyed (SDestroyCtx &Ctx);
		void FireOnSystemWeaponFire (CSpaceObject *pShot, CSpaceObject *pSource, DWORD dwItemUNID, int iRepeatingCount);
		bool FireOnTranslateMessage (const CString &sMessage, CString *retsMessage);
		void FireOnUpdate (void);
		inline DWORD GetAPIVersion (void) const { CDesignType *pType = GetType(); return (pType ? pType->GetAPIVersion() : API_VERSION); }
		void GetBoundingRect (CVector *retvUR, CVector *retvLL);
		inline CVector GetBoundsDiag (void) { return CVector(m_rBoundsX, m_rBoundsY); }
		inline Metric GetBoundsRadius (void) const { return Max(m_rBoundsX, m_rBoundsY); }
		CCommunicationsHandler *GetCommsHandler (void);
		int GetCommsMessageCount (void);
		inline const CString &GetData (const CString &sAttrib) const { return m_Data.GetData(sAttrib); }
		inline CString GetDataField (const CString &sField) { CString sValue; FindDataField(sField, &sValue); return sValue; }
		int GetDataInteger (const CString &sAttrib) const;
		CString GetDesiredCommsKey (void) const;
		inline int GetDestiny (void) const { return m_iDestiny; }
		inline Metric GetDetectionRange (int iPerception) { return CPerceptionCalc::GetRange(GetDetectionRangeIndex(iPerception)); }
		Metric GetDetectionRange2 (int iPerception) const;
		int GetDetectionRangeIndex (int iPerception) const;
		CSovereign::Disposition GetDispositionTowards (CSpaceObject *pObj);
		Metric GetDistance (CSpaceObject *pObj) const { return (pObj->GetPos() - GetPos()).Length(); }
		Metric GetDistance2 (CSpaceObject *pObj) const { return (pObj->GetPos() - GetPos()).Length2(); }
		CDesignType *GetFirstDockScreen (CString *retsScreen, ICCItem **retpData);
		inline const CString &GetHighlightText (void) const { return m_sHighlightText; }
		void GetHitRect (CVector *retvUR, CVector *retvLL);
		Metric GetHitSize (void) const;
		inline int GetHitSizeHalfAngle (Metric rDist) const { return Max((int)(180.0 * atan(0.5 * GetHitSize() / rDist) / PI), 1); }
		inline DWORD GetID (void) const { return m_dwID; }
		inline int GetIndex (void) const { return m_iIndex; }
		CSpaceObject *GetNearestEnemy (Metric rMaxRange = g_InfiniteDistance, bool bIncludeStations = false);
		CSpaceObject *GetNearestEnemyStation (Metric rMaxRange = g_InfiniteDistance);
		CSpaceObject *GetNearestStargate (bool bExcludeUncharted = false);

#define FLAG_INCLUDE_NON_AGGRESSORS		0x00000001
#define FLAG_INCLUDE_STATIONS			0x00000002
		int GetNearestVisibleEnemies (int iMaxEnemies, 
									  Metric rMaxDist, 
									  TArray<CSpaceObject *> *pretList, 
									  CSpaceObject *pExcludeObj = NULL,
									  DWORD dwFlags = 0);

		CSpaceObject *GetNearestVisibleEnemy (Metric rMaxRange = g_InfiniteDistance, bool bIncludeStations = false, CSpaceObject *pExcludeObj = NULL);
		CSpaceObject *GetNearestVisibleEnemyInArc (int iMinFireArc, int iMaxFireArc, Metric rMaxRange = g_InfiniteDistance, bool bIncludeStations = false, CSpaceObject *pExcludeObj = NULL);
		CString GetNounPhrase (DWORD dwFlags = 0);
		inline CSpaceObject *GetObjRefData (const CString &sAttrib) { return m_Data.GetObjRefData(sAttrib); }
		CSpaceObject *GetOrderGiver (DestructionTypes iCause = killedNone);
		inline CDesignType *GetOverride (void) { return m_pOverride; }
		inline CSpaceObject *GetPlayerShip (void) const { return (m_pSystem ? m_pSystem->GetPlayerShip() : NULL); }
		inline const CVector &GetPos (void) const { return m_vPos; }
		CSovereign *GetSovereignToDefend (void) const;
		const CString &GetStaticData (const CString &sAttrib);
		CG32bitPixel GetSymbolColor (void);
		void GetVisibleEnemies (DWORD dwFlags, TArray<CSpaceObject *> *retList, CSpaceObject *pExcludeObj = NULL);
		CSpaceObject *GetVisibleEnemyInRange (CSpaceObject *pCenter, Metric rMaxRange = g_InfiniteDistance, bool bIncludeStations = false, CSpaceObject *pExcludeObj = NULL);
		bool HasBeenHitLately (int iTicks = 30);
		bool HasFiredLately (int iTicks = 30);
		inline bool HasGetDockScreenEvent (void) const { return (m_fHasGetDockScreenEvent ? true : false); }
		bool HasGravity (void) const { return (m_fHasGravity ? true : false); }
		inline bool HasInterSystemEvent (void) const { return (m_fHasInterSystemEvent ? true : false); }
		inline bool HasNonLinearMove (void) const { return (m_fNonLinearMove ? true : false); }
		inline bool HasOnAttackedEvent (void) const { return (m_fHasOnAttackedEvent ? true : false); }
		inline bool HasOnAttackedByPlayerEvent (void) const { return (m_fHasOnAttackedByPlayerEvent ? true : false); }
		inline bool HasOnDamageEvent (void) const { return (m_fHasOnDamageEvent ? true : false); }
		inline bool HasOnObjDockedEvent (void) const { return (m_fHasOnObjDockedEvent ? true : false); }
		inline bool HasOnOrderChangedEvent (void) const { return (m_fHasOnOrderChangedEvent ? true : false); }
		inline bool HasOnOrdersCompletedEvent (void) const { return (m_fHasOnOrdersCompletedEvent ? true : false); }
		inline bool HasOnSubordinateAttackedEvent (void) const { return (m_fHasOnSubordinateAttackedEvent ? true : false); }
		inline bool HasRandomEncounters (void) const { int iFreq; return (GetRandomEncounterTable(&iFreq) || iFreq > 0); }
		void Highlight (const CString &sText = NULL_STR);
		inline bool HitSizeInBox (const CVector &vUR, const CVector &vLL)
			{
			Metric rHalfSize = 0.5 * GetHitSize();
			return (vUR.GetX() > m_vPos.GetX() - rHalfSize) 
					&& (vUR.GetY() > m_vPos.GetY() - rHalfSize)
					&& (vLL.GetX() < m_vPos.GetX() + rHalfSize)
					&& (vLL.GetY() < m_vPos.GetY() + rHalfSize);
			}
		bool InBarrier (const CVector &vPos);
		inline bool InBox (const CVector &vUR, const CVector &vLL) const 
			{ return (vUR.GetX() > m_vPos.GetX() - m_rBoundsX) 
					&& (vUR.GetY() > m_vPos.GetY() - m_rBoundsY)
					&& (vLL.GetX() < m_vPos.GetX() + m_rBoundsX)
					&& (vLL.GetY() < m_vPos.GetY() + m_rBoundsY); }
		inline bool InBoxPoint (const CVector &vUR, const CVector &vLL) const
			{ return (vUR.GetX() > m_vPos.GetX()) 
					&& (vUR.GetY() > m_vPos.GetY())
					&& (vLL.GetX() < m_vPos.GetX())
					&& (vLL.GetY() < m_vPos.GetY()); }
		inline void IncData (const CString &sAttrib, ICCItem *pValue = NULL, ICCItem **retpNewValue = NULL) { m_Data.IncData(sAttrib, pValue, retpNewValue); }
		bool InteractsWith (int iInteraction) const;
		bool IsAutoClearDestination (void) { return m_fAutoClearDestination; }
		bool IsAutoClearDestinationOnDestroy (void) { return m_fAutoClearDestinationOnDestroy; }
		bool IsAutoClearDestinationOnDock (void) { return m_fAutoClearDestinationOnDock; }
		bool IsAutomatedWeapon (void) const { return (m_fAutomatedWeapon ? true : false); }
		bool IsBarrier (void) const { return (m_fIsBarrier ? true : false); }
		inline bool IsCollisionTestNeeded (void) { return m_fCollisionTestNeeded; }
		bool IsCommsMessageValidFrom (CSpaceObject *pSender, int iIndex, CString *retsMsg = NULL, CString *retsKey = NULL);
		bool IsCovering (CSpaceObject *pObj);
		bool IsCreated (void) { return m_fOnCreateCalled; }
		bool IsDestinyTime (int iCycle, int iOffset = 0);
		bool IsDestroyed (void) const { return (m_fDestroyed ? true : false); }
		static bool IsDestroyedInUpdate (void) { return m_bObjDestroyed; }
		bool IsEnemy (const CSpaceObject *pObj) const;
		bool IsEnemy (const CDamageSource &Obj) const;
		bool IsEnemyInRange (Metric rMaxRange, bool bIncludeStations = false);
		bool IsEscortingFriendOf (const CSpaceObject *pObj) const;
		bool IsFriend (const CSpaceObject *pObj) const;
		inline bool IsHighlighted (void) { return ((m_iHighlightCountdown != 0) || m_fSelected || m_iHighlightChar); }
		bool IsInDamageCode (void) { return (m_fInDamage ? true : false); }
		bool IsLineOfFireClear (CInstalledDevice *pWeapon, CSpaceObject *pTarget, int iAngle, Metric rDistance = (30.0 * LIGHT_SECOND));
		inline bool IsMarked (void) const { return m_fMarked; }
		inline bool IsNamed (void) const { return m_fHasName; }
		inline bool IsOutOfPlaneObj (void) const { return m_fOutOfPlaneObj; }
		inline bool IsPaintNeeded (void) { return m_fPaintNeeded; }
		bool IsPlayerAttackJustified (void) const;
		inline bool IsPlayerDestination (void) { return m_fPlayerDestination; }
		inline bool IsPlayerDocked (void) { return m_fPlayerDocked; }
		bool IsPlayerEscortTarget (CSpaceObject *pPlayer = NULL);
		inline bool IsPlayerTarget (void) { return m_fPlayerTarget; }
		inline bool IsSelected (void) { return m_fSelected; }
		inline bool IsShowingDamageBar (void) { return m_fShowDamageBar; }
		inline bool IsShowingDistanceAndBearing (void) { return m_fShowDistanceAndBearing; }
		inline bool IsShowingHighlight (void) { return m_fShowHighlight; }
		bool IsStargateInRange (Metric rMaxRange);
		inline bool IsSubscribedToEvents (CSpaceObject *pObj) { return m_SubscribedObjs.FindObj(pObj); }
		inline bool IsTimeStopped (void) { return m_fTimeStop; }
		bool IsUnderAttack (void);
		inline void LoadObjReferences (CSystem *pSystem) { m_Data.LoadObjReferences(pSystem); }
		void NotifyOnNewSystem (CSystem *pNewSystem);
		void NotifyOnObjDestroyed (SDestroyCtx &Ctx);
		void NotifyOnObjDocked (CSpaceObject *pDockTarget);
		inline bool NotifyOthersWhenDestroyed (void) { return (m_fNoObjectDestructionNotify ? false : true); }
		void OnObjDestroyed (const SDestroyCtx &Ctx);
		void Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		void PaintHighlightText (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx, AlignmentStyles iAlign, CG32bitPixel rgbColor, int *retcyHeight = NULL);
		void PaintMap (CMapViewportCtx &Ctx, CG32bitImage &Dest, int x, int y);
		inline void PaintSRSEnhancements (CG32bitImage &Dest, SViewportPaintCtx &Ctx) { OnPaintSRSEnhancements(Dest, Ctx); }
		inline bool PointInHitSizeBox (const CVector &vPos, Metric rRadius = 0.0) const
			{ 
			CVector vRelPos = vPos - GetPos();
			Metric rHalfSize = 0.5 * GetHitSize() + rRadius;
			return ((Absolute(vRelPos.GetX()) <= rHalfSize) && (Absolute(vRelPos.GetY()) <= rHalfSize));
			}
		inline bool PosInBox (const CVector &vUR, const CVector &vLL) const
			{ return (vUR.GetX() > m_vPos.GetX()) 
					&& (vUR.GetY() > m_vPos.GetY())
					&& (vLL.GetX() < m_vPos.GetX())
					&& (vLL.GetY() < m_vPos.GetY()); }
		void Reconned (void);
		void RemoveAllEventSubscriptions (CSystem *pSystem, TArray<DWORD> *retRemoved = NULL);
		inline void RemoveEventSubscriber (CSpaceObject *pObj) { m_SubscribedObjs.Remove(pObj); }
		void ReportEventError (const CString &sEvent, ICCItem *pError);
		inline void RestartTime (void) { m_fTimeStop = false; }
		inline void SetAutoClearDestination (void) { m_fAutoClearDestination = true; }
		inline void SetAutoClearDestinationOnDestroy (void) { m_fAutoClearDestinationOnDestroy = true; }
		inline void SetAutoClearDestinationOnDock (void) { m_fAutoClearDestinationOnDock = true; }
		inline void SetAutomatedWeapon (void) { m_fAutomatedWeapon = true; }
		inline void SetCollisionTestNeeded (bool bNeeded = true) { m_fCollisionTestNeeded = bNeeded; }
		inline void SetData (const CString &sAttrib, const CString &sData) { m_Data.SetData(sAttrib, sData); }
		inline void SetDataFromDataBlock (const CAttributeDataBlock &Block) { m_Data.MergeFrom(Block); }
		inline void SetDataFromXML (CXMLElement *pData) { m_Data.SetFromXML(pData); }
		void SetDataInteger (const CString &sAttrib, int iValue);
		inline void SetDestructionNotify (bool bNotify = true) { m_fNoObjectDestructionNotify = !bNotify; }
		void SetEventFlags (void);
		inline void SetHasGetDockScreenEvent (bool bHasEvent) { m_fHasGetDockScreenEvent = bHasEvent; }
		inline void SetHasOnAttackedEvent (bool bHasEvent) { m_fHasOnAttackedEvent = bHasEvent; }
		inline void SetHasOnAttackedByPlayerEvent (bool bHasEvent) { m_fHasOnAttackedByPlayerEvent = bHasEvent; }
		inline void SetHasOnDamageEvent (bool bHasEvent) { m_fHasOnDamageEvent = bHasEvent; }
		inline void SetHasInterSystemEvent (bool bHasEvent) { m_fHasInterSystemEvent = bHasEvent; }
		inline void SetHasOnObjDockedEvent (bool bHasEvent) { m_fHasOnObjDockedEvent = bHasEvent; }
		inline void SetHasOnOrderChangedEvent (bool bHasEvent) { m_fHasOnOrderChangedEvent = bHasEvent; }
		inline void SetHasOnOrdersCompletedEvent (bool bHasEvent) { m_fHasOnOrdersCompletedEvent = bHasEvent; }
		inline void SetHasOnSubordinateAttackedEvent (bool bHasEvent) { m_fHasOnSubordinateAttackedEvent = bHasEvent; }
		inline void SetHighlightChar (char chChar) { m_iHighlightChar = chChar; }
		inline void SetMarked (bool bMarked = true) { m_fMarked = bMarked; }
		inline void SetNamed (bool bNamed = true) { m_fHasName = bNamed; }
		inline void SetObjRefData (const CString &sAttrib, CSpaceObject *pObj) { m_Data.SetObjRefData(sAttrib, pObj); }
		inline void SetOutOfPlaneObj (bool bValue = true) { m_fOutOfPlaneObj = bValue; }
		void SetOverride (CDesignType *pOverride);
		inline void SetPainted (void) { m_fPainted = true; }
		inline void SetPaintNeeded (void) { m_fPaintNeeded = true; }
		inline void SetPlayerDestination (void) { m_fPlayerDestination = true; }
		inline void SetPlayerDocked (void) { m_fPlayerDocked = true; }
		inline void SetPlayerTarget (void) { m_fPlayerTarget = true; }
		inline bool SetPOVLRS (void)
			{
			if (m_fInPOVLRS)
				return false;

			m_fInPOVLRS = true;
			return true;
			}
		inline void SetSelection (void) { m_fSelected = true; }
		inline void SetShowDamageBar (void) { m_fShowDamageBar = true; }
		inline void SetShowDistanceAndBearing (void) { m_fShowDistanceAndBearing = true; }
		inline void SetShowHighlight (void) { m_fShowHighlight = true; }
		inline void StopTime (void) { m_fTimeStop = true; }
		void Update (SUpdateCtx &Ctx);
		void UpdateExtended (const CTimeSpan &ExtraTime);
		inline void UpdatePlayer (SUpdateCtx &Ctx) { OnUpdatePlayer(Ctx); }
		inline bool WasPainted (void) const { return m_fPainted; }
		void WriteToStream (IWriteStream *pStream);
		inline void WriteObjRefToStream (CSpaceObject *pObj, IWriteStream *pStream) { GetSystem()->WriteObjRefToStream(pObj, pStream, this); }

		bool MatchesCriteria (SCriteriaMatchCtx &Ctx, const Criteria &Crit);
		static void ParseCriteria (CSpaceObject *pSource, const CString &sCriteria, Criteria *retCriteria);
		static void SetCriteriaSource (Criteria &Crit, CSpaceObject *pSource);

#ifdef DEBUG_VECTOR
		void PaintDebugVector (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		inline void SetDebugVector (const CVector &vVector) { m_vDebugVector = vVector; }
#else
		inline void PaintDebugVector (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) { }
		inline void SetDebugVector (const CVector &vVector) { }
#endif
		//	Motion
		//
		//	CanMove: This returns TRUE if OnMove should be called for the object.
		//		Note that ships need to be called at OnMove even if they are
		//		anchored because they update their particle effects during Move.
		//		But for most cases, using IsAnchored is better than CanMove.
		//
		//	IsAnchored: Anchored objects don't move even if pushed e.g., by
		//		momentum or gravity). Objects can be temporarily anchored
		//		(e.g., when docked), so it is possible for an object to return
		//		TRUE for both CanThrust and IsAnchored.

		virtual bool CanMove (void) const { return !IsAnchored(); }
		virtual bool CanThrust (void) const { return false; }
		virtual Metric GetMaxSpeed (void) { return (IsAnchored() ? 0.0 : MAX_SYSTEM_SPEED); }
		virtual bool IsAnchored (void) const { return IsManuallyAnchored(); }

		void Accelerate (const CVector &vPush, Metric rSeconds);
		void AccelerateStop (Metric rPush, Metric rSeconds);
		inline void ClipSpeed (Metric rMaxSpeed) { m_vVel.Clip(rMaxSpeed); }
		inline const CVector &DeltaV (const CVector &vDelta) { m_vVel = m_vVel + vDelta; return m_vVel; }
		inline const CVector &GetOldPos (void) const { return m_vOldPos; }
		inline const CVector &GetVel (void) const { return m_vVel; }
		inline bool IsInsideBarrier (void) const { return m_fInsideBarrier; }
		inline bool IsManuallyAnchored (void) const { return m_fManualAnchor; }
		void Jump (const CVector &vPos);
		void Move (SUpdateCtx &Ctx, Metric rSeconds);
		inline void Place (const CVector &vPos, const CVector &vVel = NullVector) { CVector vOldPos = m_vPos; m_vPos = vPos; m_vOldPos = vPos; m_vVel = vVel; OnPlace(vOldPos); }
		inline void SetInsideBarrier (bool bInside = true) { m_fInsideBarrier = bInside; }
		inline void SetManualAnchor (bool bAnchored = true) { m_fManualAnchor = bAnchored; }
		inline void SetPos (const CVector &vPos) { m_vPos = vPos; }
		inline void SetVel (const CVector &vVel) { m_vVel = vVel; }

		//	Overlays

		virtual void AddOverlay (COverlayType *pType, int iPosAngle, int iPosRadius, int iRotation, int iLifetime, DWORD *retdwID = NULL) { if (retdwID) *retdwID = 0; }
		virtual COverlayList *GetOverlays (void) { return NULL; }
		virtual void RemoveOverlay (DWORD dwID) { }

		void AddOverlay (COverlayType *pType, const CVector &vPos, int iRotation, int iLifetime, DWORD *retdwID = NULL);
		void CalcOverlayPos (COverlayType *pOverlayType, const CVector &vPos, int *retiPosAngle, int *retiPosRadius);
		inline COverlay *GetOverlay (DWORD dwID) { COverlayList *pOverlays = GetOverlays(); return (pOverlays ? pOverlays->GetOverlay(dwID) : NULL); }
		inline const CString &GetOverlayData (DWORD dwID, const CString &sAttrib) { COverlayList *pOverlays = GetOverlays(); return (pOverlays ? pOverlays->GetData(dwID, sAttrib) : NULL_STR); }
		inline void GetOverlayImpact (COverlayList::SImpactDesc *retImpact) { COverlayList *pOverlays = GetOverlays(); if (pOverlays) pOverlays->GetImpact(this, retImpact); else *retImpact = COverlayList::SImpactDesc(); }
		inline void GetOverlayList (TArray<COverlay *> *retList) { COverlayList *pOverlays = GetOverlays(); if (pOverlays) pOverlays->GetList(retList); else retList->DeleteAll(); }
		inline void GetOverlayListOfCommandPaneCounters (TArray<COverlay *> *retList) { COverlayList *pOverlays = GetOverlays(); if (pOverlays) pOverlays->GetListOfCommandPaneCounters(retList); else retList->DeleteAll(); }
		inline CVector GetOverlayPos (DWORD dwID) { COverlayList *pOverlays = GetOverlays(); return (pOverlays ? pOverlays->GetPos(this, dwID) : GetPos()); }
		ICCItem *GetOverlayProperty (CCodeChainCtx *pCCCtx, DWORD dwID, const CString &sName);
		inline int GetOverlayRotation (DWORD dwID) { COverlayList *pOverlays = GetOverlays(); return (pOverlays ? pOverlays->GetRotation(dwID) : -1); }
		inline COverlayType *GetOverlayType (DWORD dwID) { COverlayList *pOverlays = GetOverlays(); return (pOverlays ? pOverlays->GetType(dwID) : NULL); }
		inline void SetOverlayData (DWORD dwID, const CString &sAttribute, const CString &sData) { COverlayList *pOverlays = GetOverlays(); if (pOverlays) pOverlays->SetData(dwID, sAttribute, sData); }
		inline bool SetOverlayEffectProperty (DWORD dwID, const CString &sProperty, ICCItem *pValue) { COverlayList *pOverlays = GetOverlays(); return (pOverlays ? pOverlays->SetEffectProperty(dwID, sProperty, pValue) : false); }
		inline void SetOverlayPos (DWORD dwID, const CVector &vPos) { COverlayList *pOverlays = GetOverlays(); if (pOverlays) pOverlays->SetPos(this, dwID, vPos); }
		inline bool SetOverlayProperty (DWORD dwID, const CString &sName, ICCItem *pValue, CString *retsError) { COverlayList *pOverlays = GetOverlays(); return (pOverlays ? pOverlays->SetProperty(this, dwID, sName, pValue) : false); }
		inline void SetOverlayRotation (DWORD dwID, int iRotation) { COverlayList *pOverlays = GetOverlays(); if (pOverlays) pOverlays->SetRotation(dwID, iRotation); }

		//	Trade

		virtual CTradingDesc *AllocTradeDescOverride (void) { return NULL; }
		virtual CCurrencyBlock *GetCurrencyBlock (bool bCreate = false) { return NULL; }
		virtual CTradingDesc *GetTradeDescOverride (void) { return NULL; }
		virtual CCurrencyAndValue GetTradePrice (CSpaceObject *pProvider) { return CCurrencyAndValue(0, GetDefaultEconomy()); }

		void AddBuyOrder (CItemType *pType, const CString &sCriteria, int iPriceAdj);
		void AddSellOrder (CItemType *pType, const CString &sCriteria, int iPriceAdj);
		CurrencyValue ChargeMoney (DWORD dwEconomyUNID, CurrencyValue iValue);
		CurrencyValue CreditMoney (DWORD dwEconomyUNID, CurrencyValue iValue);
		bool GetArmorInstallPrice (const CItem &Item, DWORD dwFlags, int *retiPrice, CString *retsReason = NULL);
		bool GetArmorRepairPrice (CSpaceObject *pSource, const CItem &Item, int iHPToRepair, DWORD dwFlags, int *retiPrice);
		CurrencyValue GetBalance (DWORD dwEconomyUNID);
		int GetBuyPrice (const CItem &Item, DWORD dwFlags, int *retiMaxCount = NULL);
		CEconomyType *GetDefaultEconomy (void);
		DWORD GetDefaultEconomyUNID (void);
		bool GetDeviceInstallPrice (const CItem &Item, DWORD dwFlags, int *retiPrice, CString *retsReason = NULL, DWORD *retdwPriceFlags = NULL);
		bool GetDeviceRemovePrice (const CItem &Item, DWORD dwFlags, int *retiPrice, DWORD *retdwPriceFlags = NULL);
		bool GetRefuelItemAndPrice (CSpaceObject *pObjToRefuel, CItemType **retpItemType, int *retiPrice);
		int GetSellPrice (const CItem &Item, DWORD dwFlags);
		bool GetShipBuyPrice (CSpaceObject *pShip, DWORD dwFlags, int *retiPrice);
		bool GetShipSellPrice (CSpaceObject *pShip, DWORD dwFlags, int *retiPrice);
		int GetTradeMaxLevel (ETradeServiceTypes iService);
		bool HasTradeService (ETradeServiceTypes iService);
		bool HasTradeUpgradeOnly (ETradeServiceTypes iService);
		void RecordBuyItem (CSpaceObject *pSellerObj, const CItem &Item, const CCurrencyAndValue &Price);
		void SetTradeDesc (CEconomyType *pCurrency, int iMaxCurrency, int iReplenishCurrency);

		//	Ships

		virtual const CShipPerformanceDesc &GetShipPerformance (void) const { return CShipPerformanceDesc::Null(); }

		//	Stargates (object is a stargate)

		virtual CString GetStargateID (void) const { return NULL_STR; }
		virtual bool IsActiveStargate (void) const { return false; }
		virtual bool IsStargate (void) const { return false; }
		virtual bool RequestGate (CSpaceObject *pObj);
		virtual bool SupportsGating (void) { return false; }

		Metric GetMaxGateDist2 (void) const;

		//	Statics

		static int ConvertToCompatibleIndex (const CItem &Item, InstallItemResults iResult);
		static CString ConvertToID (InstallItemResults iResult);

		//	Other virtuals to be overridden

		//	...for all objects

		virtual CBoundaryMarker *AsBoundaryMarker (void) { return NULL; }
		virtual CMissile *AsMissile (void) { return NULL; }
		virtual CMission *AsMission (void) { return NULL; }
		virtual CShip *AsShip (void) { return NULL; }
		virtual CStation *AsStation (void) { return NULL; }
		virtual bool CalcVolumetricShadowLine (SLightingCtx &Ctx, int *retxCenter, int *retyCenter, int *retiWidth, int *retiLength) { return false; }
		virtual bool CanAttack (void) const { return false; }
		virtual bool CanBeDestroyed (void) { return true; }
		virtual bool CanBeHitBy (const DamageDesc &Damage) { return true; }
		virtual bool CanHit (CSpaceObject *pObj) { return true; }
		virtual bool ClassCanAttack (void) { return false; }
		virtual void CreateStarlightImage (int iStarAngle, Metric rStarDist) { }
		virtual CString DebugCrashInfo (void) { return NULL_STR; }
		virtual bool FindDataField (const CString &sField, CString *retsValue) { return false; }
		virtual AbilityStatus GetAbility (Abilities iAbility) { return ablUninstalled; }
		virtual Categories GetCategory (void) const { return catOther; }
		virtual DWORD GetClassUNID (void) { return 0; }
		virtual const CString &GetGlobalData (const CString &sAttribute) { return NULL_STR; }
		virtual Metric GetGravity (Metric *retrRadius) const { return 0.0; }
		virtual const CObjectImageArray &GetHeroImage (void) const { static CObjectImageArray NullImage; return NullImage; }
		virtual const CObjectImageArray &GetImage (void) const;
        virtual const CCompositeImageSelector &GetImageSelector (void) const { return CCompositeImageSelector::Null(); }
		virtual int GetInteraction (void) const { return 100; }
		virtual Metric GetInvMass (void) const { return 0.0; }
		virtual const COrbit *GetMapOrbit (void) const { return NULL; }
		virtual Metric GetMass (void) const { return 0.0; }
		virtual CString GetName (DWORD *retdwFlags = NULL) { if (retdwFlags) *retdwFlags = 0; return LITERAL("unknown object"); }
		virtual CString GetObjClassName (void) { return CONSTLIT("unknown"); }
		virtual CSystem::LayerEnum GetPaintLayer (void) { return CSystem::layerStations; }
		virtual Metric GetParallaxDist (void) { return 0.0; }
		virtual EDamageResults GetPassthroughDefault (void) { return damageNoDamage; }
		virtual int GetPlanetarySize (void) const { return 0; }
		virtual ICCItem *GetProperty (CCodeChainCtx &Ctx, const CString &sName);
		virtual ScaleTypes GetScale (void) const { return scaleFlotsam; }
		virtual CSovereign *GetSovereign (void) const { return NULL; }
		virtual Metric GetStellarMass (void) const { return 0.0; }
		virtual CDesignType *GetType (void) const { return NULL; }
		virtual CDesignType *GetWreckType (void) const { return NULL; }
		virtual bool HasAttribute (const CString &sAttribute) const { return sAttribute.IsBlank(); }
		virtual bool HasSpecialAttribute (const CString &sAttrib) const;
		virtual bool HasVolumetricShadow (void) const { return false; }
		virtual bool IsExplored (void) { return true; }
		virtual bool IsImmutable (void) const { return false; }
		virtual bool IsKnown (void) { return true; }
		virtual bool IsMarker (void) { return false; }
		virtual bool IsMission (void) { return false; }
		virtual bool IsNonSystemObj (void) { return false; }
		virtual bool IsShownInGalacticMap (void) const { return false; }
		virtual bool IsVirtual (void) const { return false; }
		virtual bool IsWreck (void) const { return false; }
		virtual void MarkImages (void) { }
		virtual void OnPlayerChangedShips (CSpaceObject *pOldShip, SPlayerChangedShipsCtx &Options) { }
		virtual void OnSystemCreated (SSystemCreateCtx &CreateCtx) { }
		virtual void OnSystemLoaded (void) { }
		virtual void PaintLRSBackground (CG32bitImage &Dest, int x, int y, const ViewportTransform &Trans) { }
		virtual void PaintLRSForeground (CG32bitImage &Dest, int x, int y, const ViewportTransform &Trans);
		virtual bool PointInObject (const CVector &vObjPos, const CVector &vPointPos) { return false; }
		virtual bool PointInObject (SPointInObjectCtx &Ctx, const CVector &vObjPos, const CVector &vPointPos) { return PointInObject(vObjPos, vPointPos); }
		virtual void PointInObjectInit (SPointInObjectCtx &Ctx) { }
		virtual void RefreshBounds (void) { }
		virtual bool SetAbility (Abilities iAbility, AbilityModifications iModification, int iDuration, DWORD dwOptions) { return false; }
		virtual void SetExplored (bool bExplored = true) { }
		virtual void SetGlobalData (const CString &sAttribute, const CString &sData) { }
		virtual void SetKnown (bool bKnown = true) { }
		virtual void SetName (const CString &sName, DWORD dwFlags = 0) { }
		virtual bool SetProperty (const CString &sName, ICCItem *pValue, CString *retsError);

		virtual bool IsHidden (void) const { return false; }
		virtual bool IsInactive (void) const { return IsSuspended(); }
		virtual bool IsIntangible (void) const { return (IsVirtual() || IsSuspended() || IsDestroyed()); }
		virtual bool IsSuspended (void) const { return false; }

		//	...for active/intelligent objects (ships, stations, etc.)

		virtual bool CanInstallItem (const CItem &Item, int iSlot = -1, InstallItemResults *retiResult = NULL, CString *retsResult = NULL, CItem *retItemToReplace = NULL);
		virtual void DamageExternalDevice (int iDev, SDamageCtx &Ctx) { }
		virtual void DeactivateShields (void) { }
		virtual void Decontaminate (void) { }
		virtual void DepleteShields (void) { }
		virtual void DisableDevice (CInstalledDevice *pDevice) { }
		virtual CInstalledArmor *FindArmor (const CItem &Item) { return NULL; }
		virtual CInstalledDevice *FindDevice (const CItem &Item) { return NULL; }
		virtual bool FindDeviceSlotDesc (const CItem &Item, SDeviceDesc *retDesc) { return false; }
		virtual int GetAISettingInteger (const CString &sSetting) { return 0; }
		virtual CString GetAISettingString (const CString &sSetting) { return NULL_STR; }
		virtual CArmorSystem *GetArmorSystem (void) { return NULL; }
		virtual Metric GetCargoSpaceLeft (void) { return 1000000.0; }
		virtual int GetCombatPower (void) { return 0; }
		virtual int GetCyberDefenseLevel (void) { return GetLevel(); }
		virtual int GetDamageEffectiveness (CSpaceObject *pAttacker, CInstalledDevice *pWeapon) { return 0; }
		virtual DamageTypes GetDamageType (void) { return damageGeneric; }
		virtual CSpaceObject *GetDestination (void) const { return NULL; }
		virtual CInstalledDevice *GetDevice (int iDev) const { return NULL; }
		virtual int GetDeviceCount (void) const { return 0; }
		virtual CStationType *GetEncounterInfo (void) { return NULL; }
		virtual CSpaceObject *GetEscortPrincipal (void) const { return NULL; }
		virtual int GetLastFireTime (void) const { return 0; }
		virtual int GetLastHitTime (void) const { return 0; }
		virtual int GetLevel (void) const { return 1; }
		virtual int GetMaxPower (void) const { return 0; }
		virtual int GetMaxLightDistance (void) { return 0; }
		virtual Metric GetMaxWeaponRange (void) const { return 0.0; }
		virtual CInstalledDevice *GetNamedDevice (DeviceNames iDev) const { return NULL; }
		virtual int GetPerception (void) { return perceptNormal; }
		virtual CSpaceObject *GetTarget (CItemCtx &ItemCtx, bool bNoAutoTarget = false) const { return NULL; }
		virtual int GetScore (void) { return 0; }
		virtual int GetShieldLevel (void) { return -1; }
		virtual CG32bitPixel GetSpaceColor (void) { return 0; }
		virtual int GetStealth (void) const { return stealthNormal; }
		virtual int GetVisibleDamage (void) { return 0; }
		virtual void GetVisibleDamageDesc (SVisibleDamage &Damage) { Damage = SVisibleDamage(); }
		virtual bool HasMapLabel (void) { return false; }
		virtual bool IsAngry (void) { return false; }
		virtual bool IsAngryAt (CSpaceObject *pObj) { return IsEnemy(pObj); }
		virtual bool IsBlind (void) { return false; }
		virtual bool IsDisarmed (void) { return false; }
		virtual bool IsIdentified (void) { return true; }
		virtual bool IsMultiHull (void) { return false; }
		virtual bool IsParalyzed (void) { return false; }
		virtual bool IsPlayer (void) const { return false; }
		virtual bool IsRadioactive (void) { return false; }
		virtual bool IsTimeStopImmune (void) { return false; }
		virtual void MakeBlind (int iTickCount = -1) { }
		virtual void MakeParalyzed (int iTickCount = -1) { }
		virtual void MakeRadioactive (void) { }
		virtual void OnComponentChanged (ObjectComponentTypes iComponent) { }
		virtual bool OnDestroyCheck (DestructionTypes iCause, const CDamageSource &Attacker) { return true; }
		virtual void OnDeviceStatus (CInstalledDevice *pDev, CDeviceClass::DeviceNotificationTypes iEvent) { }
		virtual void OnHitByDeviceDamage (void) { }
		virtual void OnHitByDeviceDisruptDamage (DWORD dwDuration) { }
		virtual void OnHitByRadioactiveDamage (SDamageCtx &Ctx) { }
		virtual void OnMissionCompleted (CMission *pMission, bool bSuccess) { }
		virtual void OnObjDamaged (SDamageCtx &Ctx) { }
		virtual void OnObjDestroyedNotify (SDestroyCtx &Ctx) { FireOnObjDestroyed(Ctx); }
		virtual void OnObjLeaveGate (CSpaceObject *pObj) { }
		virtual void OnPlayerObj (CSpaceObject *pPlayer) { }
		virtual void OnStationDestroyed (const SDestroyCtx &Ctx) { }
		virtual void OnSubordinateDestroyed (SDestroyCtx &Ctx) { }
		virtual void OnSubordinateHit (SDamageCtx &Ctx) { }
		virtual void ProgramDamage (CSpaceObject *pHacker, const ProgramDesc &Program) { }
		virtual void SendMessage (CSpaceObject *pSender, const CString &sMsg) { }
		virtual int SetAISettingInteger (const CString &sSetting, int iValue) { return 0; }
		virtual CString SetAISettingString (const CString &sSetting, const CString &sValue) { return NULL_STR; }
		virtual void SetIdentified (bool bIdentified = true) { }
		virtual void SetMapLabelPos (int x, int y) { }
		virtual void UpdateArmorItems (void) { }

		//	...for objects that can bounce

		inline bool CanBounce (void) { return m_fCanBounce; }
		virtual bool ImageInObject (const CVector &vObjPos, const CObjectImageArray &Image, int iTick, int iRotation, const CVector &vImagePos) { return PointInObject(vObjPos, vImagePos); }
		virtual bool ObjectInObject (const CVector &vObj1Pos, CSpaceObject *pObj2, const CVector &vObj2Pos) { return PointInObject(vObj1Pos, vObj2Pos); }
		virtual void OnBounce (CSpaceObject *pBarrierObj, const CVector &vPos) { }
		virtual void OnObjBounce (CSpaceObject *pObj, const CVector &vPos) { }

		//	...for objects with devices

		virtual bool AbsorbWeaponFire (CInstalledDevice *pWeapon) { return false; }
		virtual CString GetInstallationPhrase (const CItem &Item) const { return NULL_STR; }
		virtual void SetFireDelay (CInstalledDevice *pWeapon, int iDelay = -1) { }

		//	...for beams, missiles, etc.

		virtual void AddContinuousBeam (const CVector &vPos, const CVector &vVel, int iDirection) { }
		virtual void CreateReflection (const CVector &vPos, int iDirection) { }
		virtual void DetonateNow (CSpaceObject *pHit) { }
		virtual CString GetDamageCauseNounPhrase (DWORD dwFlags) { return GetNounPhrase(dwFlags); }
		virtual const CDamageSource &GetDamageSource (void) const { return CDamageSource::Null(); }
		virtual CWeaponFireDesc *GetWeaponFireDesc (void) { return NULL; }
		virtual CSpaceObject *GetSecondarySource (void) { return NULL; }

		//	...for ships

		virtual void Behavior (SUpdateCtx &Ctx) { }
		virtual bool FollowsObjThroughGate (CSpaceObject *pLeader = NULL) { return false; }
		virtual CSpaceObject *GetBase (void) const { return NULL; }
		virtual int GetRotation (void) const { return 0; }
		virtual bool IsPlayerWingman (void) const { return false; }
		virtual void RepairDamage (int iHitPoints) { }
		virtual void Resume (void) { }
		virtual void Suspend (void) { }
		void ScrapeOverlays (void);

		//	...for stations

		virtual void AddSubordinate (CSpaceObject *pSubordinate) { }
		virtual IShipGenerator *GetRandomEncounterTable (int *retiFrequency = NULL) const { if (retiFrequency) *retiFrequency = 0; return NULL; }
		virtual bool IsAbandoned (void) const { return false; }
        virtual bool IsSatelliteSegmentOf (CSpaceObject *pBase) const { return false; }
		virtual bool RemoveSubordinate (CSpaceObject *pSubordinate) { return false; }
        virtual bool ShowMapOrbit (void) const { return false; }
        virtual bool ShowStationDamage (void) const { return false; }

		//	...for particle effects

		virtual void SetAttractor (CSpaceObject *pObj) { }

		//	...miscellaneous

		virtual void OnLosePOV (void) { }

	protected:

		//	Virtuals to be overridden

		virtual bool CanBlock (CSpaceObject *pObj) { return false; }
		virtual bool CanBlockShips (void) { return false; }
		virtual bool CanFireOn (CSpaceObject *pObj) { return true; }
		virtual void GateHook (CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate, bool bAscend) { if (!bAscend) Destroy(removedFromSystem, CDamageSource()); }
		virtual void ObjectDestroyedHook (const SDestroyCtx &Ctx) { }
		virtual void ObjectEnteredGateHook (CSpaceObject *pObjEnteredGate) { }
		virtual void OnAscended (void) { }
		virtual DWORD OnCommunicate (CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2) { return resNoAnswer; }
		virtual EDamageResults OnDamage (SDamageCtx &Ctx) { return damageNoDamage; }
		virtual void OnDestroyed (SDestroyCtx &Ctx) { }
		virtual CSpaceObject *OnGetOrderGiver (void) { return this; }
		virtual void OnItemEnhanced (CItemListManipulator &ItemList) { }
		virtual void OnMove (const CVector &vOldPos, Metric rSeconds) { }
		virtual void OnNewSystem (CSystem *pSystem) { }
		virtual void OnObjEnteredGate (CSpaceObject *pObj, CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate) { }
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) { }
		virtual void OnPaintAnnotations (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) { }
		virtual void OnPaintMap (CMapViewportCtx &Ctx, CG32bitImage &Dest, int x, int y) { }
		virtual void OnPaintSRSEnhancements (CG32bitImage &Dest, SViewportPaintCtx &Ctx) { }
		virtual void OnPlace (const CVector &vOldPos) { }
		virtual void OnReadFromStream (SLoadCtx &Ctx) { }
		virtual void OnRemoved (SDestroyCtx &Ctx) { }
		virtual void OnSetEventFlags (void) { }
		virtual void OnSetSovereign (CSovereign *pSovereign) { }
		virtual void OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick) { }
		virtual void OnUpdateExtended (const CTimeSpan &ExtraTime) { }
		virtual void OnUpdatePlayer (SUpdateCtx &Ctx) { }
		virtual void OnWriteToStream (IWriteStream *pStream) { }
		virtual bool OrientationChanged (void) { return false; }
		virtual void RevertOrientationChange (void) { }

		//	Helper functions
		void AddEffect (IEffectPainter *pPainter, int xOffset, int yOffset, int iTick = 0, int iRotation = 0);
		void CalcInsideBarrier (void);
		Metric CalculateItemMass (Metric *retrCargoMass = NULL) const;
		bool CanFireOnObjHelper (CSpaceObject *pObj);
		inline void ClearCannotBeHit (void) { m_fCannotBeHit = false; }
		inline void ClearInDamageCode (void) { m_fInDamage = false; }
		inline void ClearInUpdateCode (void) { m_pObjInUpdate = NULL; m_bObjDestroyed = false; }
		inline void ClearObjReferences (void) { m_Data.OnSystemChanged(NULL); }
		inline void ClearPainted (void) { m_fPainted = false; }
		inline void DisableObjectDestructionNotify (void) { m_fNoObjectDestructionNotify = true; }
		inline const Metric &GetBounds (void) { return m_rBoundsX; }
		CSpaceObject *HitTest (const CVector &vStart, Metric rThreshold, const DamageDesc &Damage, CVector *retvHitPos, int *retiHitDir);
		bool ImagesIntersect (const CObjectImageArray &Image1, int iTick1, int iRotation1, const CVector &vPos1,
				const CObjectImageArray &Image2, int iTick2, int iRotation2, const CVector &vPos2);
		inline bool IsObjectDestructionHooked (void) { return (m_fHookObjectDestruction ? true : false); }
		inline void ItemEnhancementModified (CItemListManipulator &ItemList) { OnItemEnhanced(ItemList); }
		bool MissileCanHitObj (CSpaceObject *pObj, CDamageSource &Source, CWeaponFireDesc *pDesc);
		void PaintEffects (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		void PaintHighlight (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		void PaintTargetHighlight (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		inline void SetObjectDestructionHook (void) { m_fHookObjectDestruction = true; }
		inline void SetCannotBeHit (void) { m_fCannotBeHit = true; }
		inline void SetCanBounce (void) { m_fCanBounce = true; }
		inline void SetBounds (Metric rBounds) { m_rBoundsX = rBounds; m_rBoundsY = rBounds; }
		inline void SetBounds (Metric rBoundsX, Metric rBoundsY) { m_rBoundsX = rBoundsX; m_rBoundsY = rBoundsY; }
		inline void SetBounds (const RECT &rcRect, Metric rParallaxDist = 1.0)
			{
			m_rBoundsX = Max(1.0, rParallaxDist) * g_KlicksPerPixel * (RectWidth(rcRect) / 2);
			m_rBoundsY = Max(1.0, rParallaxDist) * g_KlicksPerPixel * (RectHeight(rcRect) / 2);
			}
		inline void SetBounds (IEffectPainter *pPainter)
			{
			RECT rcRect;
			pPainter->GetBounds(&rcRect);
			SetBounds(rcRect);
			}
		inline void SetHasGravity (bool bGravity = true) { m_fHasGravity = bGravity; }
		inline void SetIsBarrier (void) { m_fIsBarrier = true; }
		inline void SetInDamageCode (void) { m_fInDamage = true; }
		inline void SetInUpdateCode (void) { m_pObjInUpdate = this; m_bObjDestroyed = false; }
		inline void SetNoFriendlyFire (void) { m_fNoFriendlyFire = true; }
		inline void SetNoFriendlyTarget (void) { m_fNoFriendlyTarget = true; }
		inline void SetNonLinearMove (bool bValue = true) { m_fNonLinearMove = bValue; }
		void UpdateTrade (SUpdateCtx &Ctx, int iInventoryRefreshed);
		void UpdateTradeExtended (const CTimeSpan &ExtraTime);

	private:

		struct SEffectNode
			{
			IEffectPainter *pPainter;
			int xOffset;
			int yOffset;
			int iTick;
			int iRotation;

			SEffectNode *pNext;
			};

		CSpaceObject (void);

		inline void InitItemEvents (void) { m_ItemEvents.Init(this); m_fItemEventsValid = true; }
		void UpdateEffects (void);
		void UpdatePlayerTarget (SUpdateCtx &Ctx);

		CSystem *m_pSystem;						//	Current system
		int m_iIndex;							//	Index in system
		DWORD m_dwID;							//	Universal ID
		int m_iDestiny;							//	Random number 0..DestinyRange-1
		CVector m_vPos;							//	Position of object in system
		CVector m_vVel;							//	Velocity of object
		CVector m_vOldPos;						//	Position last tick
		Metric m_rBoundsX;						//	Object bounds
		Metric m_rBoundsY;						//	Object bounds

		CString m_sHighlightText;				//	Show message text when highlighting
		int m_iHighlightCountdown:16;			//	Frames left to highlight
		int m_iHighlightChar:8;					//	Show character when painting (0 = none)
		int m_iDesiredHighlightChar:8;			//	Desired character

		CItemList m_ItemList;					//	List of items
		CAttributeDataBlock m_Data;				//	Opaque data
		SEffectNode *m_pFirstEffect;			//	List of effects
		CItemEventDispatcher m_ItemEvents;		//	Item event dispatcher
		CDesignType *m_pOverride;				//	Override event handler
		CSpaceObjectList m_SubscribedObjs;		//	List of objects to notify when something happens
		CObjectJoint *m_pFirstJoint;			//	List of joints

		int m_iControlsFrozen:8;				//	Object will not respond to controls
		int m_iSpare:24;

		DWORD m_fHookObjectDestruction:1;		//	Call the object when another object is destroyed
		DWORD m_fNoObjectDestructionNotify:1;	//	Do not call other objects when this one is destroyed
		DWORD m_fCannotBeHit:1;					//	TRUE if it cannot be hit by other objects
		DWORD m_fSelected:1;					//	TRUE if selected
		DWORD m_fInPOVLRS:1;					//	TRUE if object appears in POV's LRS
		DWORD m_fCanBounce:1;					//	TRUE if object can bounce off others
		DWORD m_fIsBarrier:1;					//	TRUE if objects bounce off this object
		DWORD m_fNoFriendlyFire:1;				//	TRUE if object cannot hit friendly objects

		DWORD m_fTimeStop:1;					//	TRUE if time has stopped for this object
		DWORD m_fPlayerTarget:1;				//	TRUE if this is a target for the player
		DWORD m_fAutomatedWeapon:1;				//	TRUE if this is a shot from automated weapon
		DWORD m_fHasOnObjDockedEvent:1;			//	TRUE if this object has an OnObjDocked event
		DWORD m_fOnCreateCalled:1;				//	TURE if OnCreate event has already been called
		DWORD m_fNoFriendlyTarget:1;			//	TRUE if object cannot be hit by friends
		DWORD m_fItemEventsValid:1;				//	TRUE if item event dispatcher is up to date
		DWORD m_fHasOnDamageEvent:1;			//	TRUE if object has OnDamage event

		DWORD m_fHasOnAttackedEvent:1;			//	TRUE if object has OnAttacked event
		DWORD m_fInDamage:1;					//	TRUE if object is inside Damage call
		DWORD m_fDestroyed:1;					//	TRUE if object is destroyed (but not yet deleted)
		DWORD m_fPlayerDestination:1;			//	TRUE if object is a destination for the player
		DWORD m_fShowDistanceAndBearing:1;		//	TRUE if we should show distance and bearing when a player destination
		DWORD m_fHasInterSystemEvent:1;			//	TRUE if object has OnPlayerEnteredSystem or OnPlayerLeftSystem event
		DWORD m_fAutoClearDestination:1;		//	TRUE if m_fPlayerDestination is cleared when object in SRS
		DWORD m_fHasOnOrdersCompletedEvent:1;	//	TRUE if object has OnOrdersCompleted event

		DWORD m_fPlayerDocked:1;				//	TRUE if player is docked with this object
		DWORD m_fPaintNeeded:1;					//	TRUE if object needs to be painted
		DWORD m_fNonLinearMove:1;				//	TRUE if object updates its position inside OnMove
		DWORD m_fHasName:1;						//	TRUE if object has been named (this is an optimization--it may have false positives)
		DWORD m_fMarked:1;						//	Temporary marker for processing lists (not persistent)
		DWORD m_fAscended:1;					//	TRUE if object is ascended (i.e., stored outside a system)
		DWORD m_fOutOfPlaneObj:1;				//	TRUE if object is out of plane
		DWORD m_fPainted:1;						//	TRUE if we painted the object last tick

		DWORD m_fAutoClearDestinationOnDock:1;	//	TRUE if we should clear the destination when player docks
		DWORD m_fShowHighlight:1;				//	TRUE if we should paint a target highlight in SRS
		DWORD m_fAutoClearDestinationOnDestroy:1;//	TRUE if we should clear the destination when station is destroyed
		DWORD m_fShowDamageBar:1;				//	TRUE if we should show damage bar
		DWORD m_fHasGravity:1;					//	TRUE if object has gravity
		DWORD m_fInsideBarrier:1;				//	TRUE if we got created inside a barrier
		DWORD m_fHasOnSubordinateAttackedEvent:1;	//	TRUE if we have a <OnSubordinateAttacked> event
		DWORD m_fHasGetDockScreenEvent:1;		//	TRUE if we have a <GetDockScreen> event

		DWORD m_fHasOnAttackedByPlayerEvent:1;	//	TRUE if we have an <OnAttackedByPlayer> event
		DWORD m_fHasOnOrderChangedEvent:1;		//	TRUE if we have an <OnOrderChanged> event
		DWORD m_fManualAnchor:1;				//	TRUE if object is temporarily anchored
		DWORD m_fCollisionTestNeeded:1;			//	TRUE if object needs to check collisions with barriers
		DWORD m_fHasDockScreenMaybe:1;			//	TRUE if object has a dock screen for player (may be stale)
		DWORD m_fSpare6:1;
		DWORD m_fSpare7:1;
		DWORD m_fSpare8:1;

		DWORD m_dwSpare:16;

#ifdef DEBUG_VECTOR
		CVector m_vDebugVector;			//	Draw a vector
#endif

		//	This is a global variable that is set when we update an object.
		//	We use it to detect when an object gets destroyed inside its
		//	own Update method.
		//
		//	Note: Obviously this only works if object updates take place
		//	on the same thread and if they are not re-entrant. (i.e., can't
		//	call Update on a object from inside the Update of a different
		//	object).

		static CSpaceObject *m_pObjInUpdate;
		static bool m_bObjDestroyed;

		//	Empty list of overlays

		static COverlayList m_NullOverlays;

	friend CObjectClass<CSpaceObject>;
	};

class CGlobalSpaceObject
	{
	public:
		CGlobalSpaceObject (void) : m_dwID(OBJID_NULL), m_pObj(NULL)
			{ }

		inline CGlobalSpaceObject &operator= (CSpaceObject *pObj)
			{
			m_dwID = (pObj ? pObj->GetID() : OBJID_NULL);
			m_pObj = pObj;
			return *this;
			}

		inline CSpaceObject *operator-> (void) { return m_pObj; }
		inline operator CSpaceObject * () const { return m_pObj; }

		inline void CleanUp (void) { m_dwID = OBJID_NULL; m_pObj = NULL; }
		inline DWORD GetID (void) const { return m_dwID; }
		inline CSpaceObject *GetObj (void) { if (IsEmpty() && m_dwID != OBJID_NULL) Resolve(); return m_pObj; }
		inline bool IsEmpty (void) const { return (m_pObj == NULL); }
		void ReadFromStream (SLoadCtx &Ctx);
		void Resolve (void);
		inline void SetID (DWORD dwID) { m_dwID = dwID; m_pObj = NULL; Resolve(); }
		void WriteToStream (IWriteStream *pStream);

	private:
		DWORD m_dwID;						//	Global ID of space object
		CSpaceObject *m_pObj;				//	Object pointer (may be NULL)
	};

class CObjectTracker
	{
	public:
		struct SObjEntry
			{
            SObjEntry (void) :
                    fKnown(false),
                    fShowDestroyed(false),
                    fShowInMap(false),
                    fFriendly(false),
                    fEnemy(false)
                { }

			CTopologyNode *pNode;
			CDesignType *pType;
			DWORD dwObjID;
			CString sName;
			DWORD dwNameFlags;
            CCompositeImageSelector ImageSel;
            CString sNotes;

            DWORD fKnown:1;
            DWORD fShowDestroyed:1;
            DWORD fShowInMap:1;
            DWORD fFriendly:1;              //  If neither friend or enemy, then neutral
            DWORD fEnemy:1;
			};

        struct SBackgroundObjEntry
            {
            CStationType *pType;
            CVector vPos;
            CCompositeImageSelector *pImageSel;
            };

		~CObjectTracker (void);

		void Delete (CSpaceObject *pObj);
		void DeleteAll (void);
		bool Find (const CString &sNodeID, const CDesignTypeCriteria &Criteria, TArray<SObjEntry> *retResult);
        void GetGalacticMapObjects (CTopologyNode *pNode, TArray<SObjEntry> &Results) const;
        void GetSystemBackgroundObjects (CTopologyNode *pNode, TSortMap<Metric, SBackgroundObjEntry> &Results) const;
        void GetSystemStarObjects (CTopologyNode *pNode, TArray<SBackgroundObjEntry> &Results) const;
        const TArray<COrbit> &GetSystemOrbits (CTopologyNode *pNode) const;
		void Insert (CSpaceObject *pObj);
		inline bool IsTracked (CSpaceObject *pObj) { CSpaceObject::Categories iCategory = pObj->GetCategory(); return (iCategory == CSpaceObject::catStation || iCategory == CSpaceObject::catShip); }
		inline void InsertIfTracked (CSpaceObject *pObj) { if (IsTracked(pObj)) Insert(pObj); }
		void ReadFromStream (SUniverseLoadCtx &Ctx);
        void Refresh (CSystem *pSystem);
		void ReplayCommands (CSystem *pSystem);
		void SetKnown (const CString &sNodeID, const CDesignTypeCriteria &Criteria, bool bKnown = true);
		void WriteToStream (IWriteStream *pStream);

	private:
		
		enum EDelayedCommandTypes
			{
			cmdUnknown =			-1,

			cmdSetKnown =			0,
			cmdSetUnknown =			1,
			};

		struct SDelayedCommand
			{
			EDelayedCommandTypes iCmd;
			CDesignTypeCriteria Criteria;
			};

		struct SObjExtra
			{
            SObjExtra (void)
                { }

			CString sName;
			DWORD dwNameFlags;
            CCompositeImageSelector ImageSel;
            CString sNotes;
			};

        struct SObjBasics
            {
            SObjBasics (void) :
                    fKnown(false),
                    fShowDestroyed(false),
                    fShowInMap(false),
                    fFriendly(false),
                    fEnemy(false),
                    pExtra(NULL)
                { }

            SObjBasics (const SObjBasics &Src) { Copy(Src); }
            ~SObjBasics (void) { CleanUp(); }

            SObjBasics &operator= (const SObjBasics &Src)
                {
                CleanUp();
                Copy(Src);
                return *this;
                }

            void CleanUp (void)
                {
                DeleteExtra();
                }

            void Copy (const SObjBasics &Src)
                {
                vPos = Src.vPos;

                fKnown = Src.fKnown;
                fShowDestroyed = Src.fShowDestroyed;
                fShowInMap = Src.fShowInMap;
                fFriendly = Src.fFriendly;
                fEnemy = Src.fEnemy;

                if (Src.pExtra)
                    pExtra = new SObjExtra(*Src.pExtra);
                else
                    pExtra = NULL;
                }

            void DeleteExtra (void)
                {
                if (pExtra)
                    {
                    delete pExtra;
                    pExtra = NULL;
                    }
                }

            SObjExtra &SetExtra (void)
                {
                if (pExtra == NULL)
                    pExtra = new SObjExtra;

                return *pExtra;
                }

            CVector vPos;                   //  Position of object in its system

            DWORD fKnown:1;                 //  TRUE if player knows about this obj
            DWORD fShowDestroyed:1;         //  TRUE if we need to paint station as destroyed
            DWORD fShowInMap:1;             //  TRUE if we can dock with the obj
            DWORD fFriendly:1;              //  If neither friend or enemy, then neutral
            DWORD fEnemy:1;
            DWORD fSpare6:1;
            DWORD fSpare7:1;
            DWORD fSpare8:1;

            DWORD dwSpare:24;

            SObjExtra *pExtra;
            };

		struct SObjList
			{
			CTopologyNode *pNode;
			CDesignType *pType;
			TSortMap<DWORD, SObjBasics> Objects;
			};

        struct SNodeData
            {
            TArray<SObjList *> ObjLists;
            TArray<COrbit> Orbits;
			TArray<SDelayedCommand> Commands;
            };

		bool AccumulateEntries (TArray<SObjList *> &Table, const CDesignTypeCriteria &Criteria, TArray<SObjEntry> *retResult);
        void AccumulateEntry (const SObjList &ObjList, DWORD dwObjID, const SObjBasics &ObjData, TArray<SObjEntry> &Results) const;
        bool Find (CTopologyNode *pNode, CSpaceObject *pObj, SObjBasics **retpObjData = NULL) const;
        bool Find (SNodeData *pNodeData, CSpaceObject *pObj, SObjBasics **retpObjData = NULL) const;
		SObjList *GetList (CSpaceObject *pObj) const;
		SObjList *GetList (CTopologyNode *pNode, CDesignType *pType) const;
        void Refresh (CSpaceObject *pObj, SObjBasics *pObjData, CSpaceObject *pPlayer);
		void ReplayCommand (const SDelayedCommand &Cmd, CSystem *pSystem);
		void SetKnown (TArray<SObjList *> &Table, const CDesignTypeCriteria &Criteria, bool bKnown);
		SObjList *SetList (CSpaceObject *pObj);
		SObjList *SetList (CTopologyNode *pNode, CDesignType *pType);
		SObjList *SetList (SNodeData *pNodeData, CTopologyNode *pNode, CDesignType *pType);

		TArray<SObjList *> m_AllLists;
		TSortMap<CString, SNodeData> m_ByNode;
	};

class CObjectStats
	{
	public:
		struct SEntry
			{
			SEntry (void) :
					iPlayerMissionsGiven(0)
				{ }

			int iPlayerMissionsGiven;		//	Number of player missions given
			};

		const SEntry &GetEntry (DWORD dwObjID) const;
		SEntry &GetEntryActual (DWORD dwObjID);
		void ReadFromStream (SUniverseLoadCtx &Ctx);
		void WriteToStream (IWriteStream *pStream);

	private:
		TSortMap<DWORD, SEntry> m_Stats;	//	Indexed by ObjID

		static SEntry m_Null;
	};

class CAscendedObjectList
	{
	public:
		~CAscendedObjectList (void) { CleanUp(); }

		inline void DeleteAll (void) { CleanUp(); }
		inline void Insert (CSpaceObject *pObj) { m_List.Insert(pObj); }
		CSpaceObject *FindByID (DWORD dwID) const;
		inline int GetCount (void) const { return m_List.GetCount(); }
		inline CSpaceObject *GetObj (int iIndex) const { return m_List[iIndex]; }
		inline bool IsEmpty (void) const { return (m_List.GetCount() == 0); }
		void ReadFromStream (SLoadCtx &Ctx);
		CSpaceObject *RemoveByID (DWORD dwID);
		void WriteToStream (IWriteStream *pStream);

	private:
		void CleanUp (void);

		TArray<CSpaceObject *> m_List;
	};

//	Fractal Texture Library ----------------------------------------------------

class CFractalTextureLibrary
	{
	public:
		enum ETextureTypes
			{
			typeNone,

			typeBoilingClouds,
			typeExplosion,
			};

		CFractalTextureLibrary (void) :
				m_bInitialized(false)
			{ }

		const CG8bitImage &GetTexture (ETextureTypes iType, int iFrame) const;
		int GetTextureCount (ETextureTypes iType) const;
		inline int GetTextureIndex (ETextureTypes iType, Metric rFraction) const
			{
			int iMaxFrames = GetTextureCount(iType);
			return Max(0, Min((int)(rFraction * iMaxFrames), iMaxFrames - 1));
			}

		void Init (void);

	private:
		void InitExplosionTextures (void);

		bool m_bInitialized;
		TArray<CG8bitImage> m_BoilingTextures;
		TArray<CG8bitImage> m_ExplosionTextures;
	};

//	IListData implementation ---------------------------------------------------

class CItemListWrapper : public IListData
	{
	public:
		CItemListWrapper (CSpaceObject *pSource);
		CItemListWrapper (CItemList &ItemList);

		virtual void DeleteAtCursor (int iCount) override { m_ItemList.DeleteAtCursor(iCount); if (m_pSource) m_pSource->InvalidateItemListAddRemove(); }
		virtual int GetCount (void) override { return m_ItemList.GetCount(); }
		virtual int GetCursor (void) override { return m_ItemList.GetCursor(); }
		virtual const CItem &GetItemAtCursor (void) override { return m_ItemList.GetItemAtCursor(); }
		virtual CItemListManipulator &GetItemListManipulator (void) override { return m_ItemList; }
		virtual CSpaceObject *GetSource (void) override { return m_pSource; }
		virtual bool IsCursorValid (void) const override { return m_ItemList.IsCursorValid(); }
		virtual bool MoveCursorBack (void) override { return m_ItemList.MoveCursorBack(); }
		virtual bool MoveCursorForward (void) override { return m_ItemList.MoveCursorForward(); }
		virtual void ResetCursor (void) override { m_ItemList.Refresh(CItem()); }
		virtual void SetCursor (int iCursor) override { m_ItemList.SetCursor(iCursor); }
		virtual void SetFilter (const CItemCriteria &Filter) override { m_ItemList.SetFilter(Filter); }
		virtual void SyncCursor (void) override { m_ItemList.SyncCursor(); }

	private:
		CSpaceObject *m_pSource;
		CItemListManipulator m_ItemList;
	};

class CListWrapper : public IListData
	{
	public:
		CListWrapper (CCodeChain *pCC, ICCItem *pList);
		virtual ~CListWrapper (void) { m_pList->Discard(m_pCC); }

		virtual int GetCount (void) override { return m_pList->GetCount(); }
		virtual int GetCursor (void) override { return m_iCursor; }
		virtual CString GetDescAtCursor (void) override;
		virtual ICCItem *GetEntryAtCursor (CCodeChain &CC) override;
		virtual CString GetTitleAtCursor (void) override;
		virtual bool IsCursorValid (void) const override { return (m_iCursor != -1); }
		virtual bool MoveCursorBack (void) override;
		virtual bool MoveCursorForward (void) override;
		virtual void PaintImageAtCursor (CG32bitImage &Dest, int x, int y, int cxWidth, int cyHeight, Metric rScale) override;
		virtual void ResetCursor (void) override { m_iCursor = -1; }
		virtual void SetCursor (int iCursor) override { m_iCursor = Min(Max(-1, iCursor), GetCount() - 1); }
		virtual void SyncCursor (void) override;

	private:
		DWORD GetImageDescAtCursor (RECT *retrcImage, Metric *retrScale) const;

		CCodeChain *m_pCC;
		ICCItem *m_pList;

		int m_iCursor;
	};

//	Implementations ------------------------------------------------------------

#include "TSEDeviceClassesImpl.h"
#include "TSESpaceObjectsImpl.h"
#include "TSEMissions.h"
#include "TSEPlayer.h"
#include "TSEArtifactAwakening.h"
#include "TSEEventsImpl.h"

//	The Universe ---------------------------------------------------------------
//
//	CREATING THE UNIVERSE
//
//	1.	Call Init() to initialize the universe with the appropriate adventure
//		and extensions. This will call Bind on all design types and initialize
//		any dynamic types.
//		[This may take a while, so it should be called on a background thread.]
//
//	2.	Call InitAdventure(), which calls OnGameStart on the adventure. This is
//		required if you need to show the crawl screen and text, otherwise it may
//		be skipped (e.g., for TransData).
//		[This may be called synchronously.]
///
//	2.	Call InitGame() to initialize the topology, etc.
//		[This should be called on a background thread.]
//
//	3.	Call StartGame(true) to start the game running.
//		[This may be called	synchronously.]

enum EStorageScopes
	{
	storeUnknown				= -1,

	storeDevice					= 0,
	storeServiceExtension		= 1,
	storeServiceUser			= 2,
	};

class CSFXOptions
	{
	public:
		enum ESFXQuality
			{
			sfxUnknown			= -1,
			sfxMinimum			= 0,
			sfxStandard			= 1,
			sfxMaximum			= 2,
			};

		CSFXOptions (void) { SetSFXQuality(sfxMaximum); }

		inline BYTE GetHUDOpacity (void) const { return (m_bHUDTransparent ? 200 : 255); }
		inline bool Is3DSystemMapEnabled (void) const { return m_b3DSystemMap; }
		inline bool IsDockScreenTransparent (void) const { return m_bDockScreenTransparent; }
		inline bool IsManeuveringEffectEnabled (void) const { return m_bManeuveringEffect; }
		inline bool IsSpaceBackgroundEnabled (void) const { return m_bSpaceBackground; }
		inline bool IsStargateTravelEffectEnabled (void) const { return m_bStargateTravelEffect; }
		inline bool IsStarGlowEnabled (void) const { return m_bStarGlow; }
		inline bool IsStarshineEnabled (void) const { return m_bStarshine; }
		inline void Set3DSystemMapEnabled (bool bEnabled = true) { m_b3DSystemMap = bEnabled; }
		inline void SetManeuveringEffectEnabled (bool bEnabled = true) { m_bManeuveringEffect = bEnabled; }
		void SetSFXQuality (ESFXQuality iQuality);
		void SetSFXQualityAuto (void);

	private:
		ESFXQuality m_iQuality;

		bool m_b3DSystemMap;				//	3D effect on system map
		bool m_bHUDTransparent;				//	HUD has transparency effect
		bool m_bManeuveringEffect;			//	Show maneuvering thruster effects
		bool m_bSpaceBackground;			//	Show system image background
		bool m_bStargateTravelEffect;		//	Show effect when changing systems
		bool m_bStarGlow;					//	Show star glow in system map
		bool m_bStarshine;					//	Show starshine effect
		bool m_bDockScreenTransparent;		//	Show SRS behind dock screen
	};

class CUniverse
	{
	public:
		class IHost
			{
			public:
				virtual void ConsoleOutput (const CString &sLine) { }
				virtual IPlayerController *CreatePlayerController (void) { return NULL; }
				virtual IShipController *CreateShipController (const CString &sController) { return NULL; }
				virtual void DebugOutput (const CString &sLine) { }
				virtual void GameOutput (const CString &sLine) { }
				virtual const CG16bitFont *GetFont (const CString &sFont) { return NULL; }
				virtual void LogOutput (const CString &sLine) const { ::kernelDebugLogString(sLine); }
			};

		class INotifications
			{
			public:
				virtual void OnObjDestroyed (SDestroyCtx &Ctx) { }
			};

		struct SInitDesc
			{
			SInitDesc (void) :
					pHost(NULL),
					bInLoadGame(false),
					bNoResources(false),
					bNoReload(false),
					bDebugMode(false),
					bDefaultExtensions(false),
					bForceTDB(false),
                    bNoCollectionLoad(false),
					bNoCollectionCheck(false),
					bForceCompatibilityLibrary(false),
					bRecoverAdventureError(false),
					bLoadObsoleteTypes(false),
					pAdventure(NULL),
					dwAdventure(0)
				{ }

			IHost *pHost;					//	Host
			TArray<SPrimitiveDefTable> CCPrimitives;	//	Additional primitives to define
			CString sFilespec;				//	Filespec of main TDB/XML file.
			CString sSourceFilespec;		//	Filespec of main source XML file (for debugging).
			CString sCollectionFolder;		//	If non-blank, use this as Collection folder (and remember it)
			TArray<CString> ExtensionFolders;	//	Add these as extension folders.
			TSortMap<DWORD, bool> TypesUsed;//	List of types used (bind these, even if obsolete).

			//	Options

			bool bInLoadGame;				//	We're loading a game, which means don't reset dynamic types
			bool bNoResources;				//	If TRUE, do not bother loaded images
			bool bNoReload;					//	If TRUE, do not reload extensions
			bool bDebugMode;				//	Initialize in debug mode
			bool bDefaultExtensions;		//	If TRUE, we include all appropriate extensions
			bool bForceTDB;					//	If TRUE, use Transcendence.tdb even if XMLs exist
			bool bNoCollectionLoad;         //  If TRUE, don't load collection (only in debug mode)
			bool bNoCollectionCheck;		//  If TRUE, don't check collection digests
			bool bForceCompatibilityLibrary;//	If TRUE, force include of Compatibility Library
			bool bRecoverAdventureError;	//	If TRUE, we try to recover if given adventure not found
			bool bLoadObsoleteTypes;		//	If TRUE, we still load obsolete types

			//	Adventure to bind to (choose one, in order)

			CExtension *pAdventure;			//	If not NULL, bind to this adventure
			DWORD dwAdventure;				//	It not 0, bind to this adventure
			CString sAdventureFilespec;		//	If not empty, bind to this adventure
			TArray<DWORD> ExtensionUNIDs;	//	Extensions to use

			//	Extension list

			TArray<CExtension *> Extensions;
			};

		enum ENamedFonts
			{
			fontMapLabel =				0,	//	Font for map labels
			fontSign =					1,	//	Font for signs
			fontSRSObjName =			2,	//	Font for name of objects in SRS
			fontSRSMessage =			3,	//	Font for comms messages from objects in SRS
			fontSRSObjCounter =			4,	//	Font for counter labels

			fontCount =					5,
			};

		CUniverse (void);
		virtual ~CUniverse (void);

		ALERROR Init (SInitDesc &Ctx, CString *retsError);
		ALERROR InitAdventure (IPlayerController *pPlayer, CString *retsError);
		ALERROR InitGame (DWORD dwStartingMap, CString *retsError);
		ALERROR LoadFromStream (IReadStream *pStream, DWORD *retdwSystemID, DWORD *retdwPlayerID, CString *retsError);
		void StartGame (bool bNewGame);

		inline void AddAscendedObj (CSpaceObject *pObj) { m_AscendedObjects.Insert(pObj); }
		inline ALERROR AddDynamicType (CExtension *pExtension, DWORD dwUNID, ICCItem *pSource, bool bNewGame, CString *retsError) { return m_Design.AddDynamicType(pExtension, dwUNID, pSource, bNewGame, retsError); }
		void AddEvent (CSystemEvent *pEvent);
		inline void AddTimeDiscontinuity (const CTimeSpan &Duration) { m_Time.AddDiscontinuity(m_iTick++, Duration); }
		ALERROR AddStarSystem (CTopologyNode *pTopology, CSystem *pSystem);
		inline bool CancelEvent (CSpaceObject *pObj, bool bInDoEvent = false) { return m_Events.CancelEvent(pObj, bInDoEvent); }
		inline bool CancelEvent (CSpaceObject *pObj, const CString &sEvent, bool bInDoEvent = false) { return m_Events.CancelEvent(pObj, sEvent, bInDoEvent); }
		inline bool CancelEvent (CDesignType *pType, const CString &sEvent, bool bInDoEvent = false) { return m_Events.CancelEvent(pType, sEvent, bInDoEvent); }
		ALERROR CreateEmptyStarSystem (CSystem **retpSystem);
		inline DWORD CreateGlobalID (void) { return m_dwNextID++; }
		ALERROR CreateMission (CMissionType *pType, CSpaceObject *pOwner, ICCItem *pCreateData, CMission **retpMission, CString *retsError);
		ALERROR CreateRandomItem (const CItemCriteria &Crit, 
								  const CString &sLevelFrequency,
								  CItem *retItem);
		ALERROR CreateRandomMission (const TArray<CMissionType *> &Types, CSpaceObject *pOwner, ICCItem *pCreateData, CMission **retpMission, CString *retsError);
		IShipController *CreateShipController (const CString &sAI);
		ALERROR CreateStarSystem (const CString &sNodeID, CSystem **retpSystem, CString *retsError = NULL, CSystemCreateStats *pStats = NULL);
		ALERROR CreateStarSystem (CTopologyNode *pTopology, CSystem **retpSystem, CString *retsError = NULL, CSystemCreateStats *pStats = NULL);
		void DestroySystem (CSystem *pSystem);
		inline CMission *FindMission (DWORD dwID) const { return m_AllMissions.GetMissionByID(dwID); }
		CSpaceObject *FindObject (DWORD dwID);
		inline void FireOnGlobalPaneInit (void *pScreen, CDesignType *pRoot, const CString &sScreen, const CString &sPane) { m_Design.FireOnGlobalPaneInit(pScreen, pRoot, sScreen, sPane); }
		inline void FireOnGlobalPlayerBoughtItem (CSpaceObject *pSellerObj, const CItem &Item, const CCurrencyAndValue &Price) { m_Design.FireOnGlobalPlayerBoughtItem(pSellerObj, Item, Price); }
		inline void FireOnGlobalPlayerChangedShips (CSpaceObject *pOldShip) { m_Design.FireOnGlobalPlayerChangedShips(pOldShip); }
		inline void FireOnGlobalPlayerEnteredSystem (void) { m_Design.FireOnGlobalPlayerEnteredSystem(); }
		inline void FireOnGlobalPlayerLeftSystem (void) { m_Design.FireOnGlobalPlayerLeftSystem(); }
		inline void FireOnGlobalPlayerSoldItem (CSpaceObject *pBuyerObj, const CItem &Item, const CCurrencyAndValue &Price) { m_Design.FireOnGlobalPlayerSoldItem(pBuyerObj, Item, Price); }
		inline void FireOnGlobalSystemCreated (SSystemCreateCtx &SysCreateCtx) { m_Design.FireOnGlobalSystemCreated(SysCreateCtx); }
		inline void FireOnGlobalUniverseCreated (void) { m_Design.FireOnGlobalUniverseCreated(); }
		inline void FireOnGlobalUniverseLoad (void) { m_Design.FireOnGlobalUniverseLoad(); }
		inline void FireOnGlobalUniverseSave (void) { m_Design.FireOnGlobalUniverseSave(); }
		void FlushStarSystem (CTopologyNode *pTopology);
		void GenerateGameStats (CGameStats &Stats);
		inline void GetAllAdventures (TArray<CExtension *> *retList) { CString sError; m_Extensions.ComputeAvailableAdventures((m_bDebugMode ? CExtensionCollection::FLAG_DEBUG_MODE : 0), retList, &sError); }
		const CDamageAdjDesc *GetArmorDamageAdj (int iLevel) const;
		inline CAscendedObjectList &GetAscendedObjects (void) { return m_AscendedObjects; }
		inline CAdventureDesc *GetCurrentAdventureDesc (void) { return m_pAdventure; }
		void GetCurrentAdventureExtensions (TArray<DWORD> *retList);
		CMission *GetCurrentMission (void);
		inline const CDisplayAttributeDefinitions &GetAttributeDesc (void) const { return m_Design.GetDisplayAttributes(); }
		inline CGImageCache &GetDynamicImageLibrary (void) { return m_DynamicImageLibrary; }
		inline CTimeSpan GetElapsedGameTime (void) { return m_Time.GetElapsedTimeAt(m_iTick); }
		inline CTimeSpan GetElapsedGameTimeAt (int iTick) { return m_Time.GetElapsedTimeAt(iTick); }
		inline CExtensionCollection &GetExtensionCollection (void) { return m_Extensions; }
		CString GetExtensionData (EStorageScopes iScope, DWORD dwExtension, const CString &sAttrib);
		CTopologyNode *GetFirstTopologyNode (void);
		inline const CG16bitFont *GetFont (const CString &sFont) { return m_pHost->GetFont(sFont); }
		inline CFractalTextureLibrary &GetFractalTextureLibrary (void) { return m_FractalTextureLibrary; }
        inline CObjectTracker &GetGlobalObjects (void) { return m_Objects; }
		inline IHost *GetHost (void) const { return m_pHost; }
		inline CMission *GetMission (int iIndex) { return m_AllMissions.GetMission(iIndex); }
		inline int GetMissionCount (void) const { return m_AllMissions.GetCount(); }
		void GetMissions (CSpaceObject *pSource, const CMission::SCriteria &Criteria, TArray<CMission *> *retList);
		inline const CG16bitFont &GetNamedFont (ENamedFonts iFont) { return *m_FontTable[iFont]; }
		inline const CObjectStats::SEntry &GetObjStats (DWORD dwObjID) const { return m_ObjStats.GetEntry(dwObjID); }
		inline CObjectStats::SEntry &GetObjStatsActual (DWORD dwObjID) { return m_ObjStats.GetEntryActual(dwObjID); }
		void GetRandomLevelEncounter (int iLevel, CDesignType **retpType, IShipGenerator **retpTable, CSovereign **retpBaseSovereign);
		inline CString GetResourceDb (void) { return m_sResourceDb; }
		inline CCriticalSection &GetSem (void) { return m_cs; }
		inline CSFXOptions &GetSFXOptions (void) { return m_SFXOptions; }
		const CDamageAdjDesc *GetShieldDamageAdj (int iLevel) const;
		inline CSoundMgr *GetSoundMgr (void) { return m_pSoundMgr; }
		inline bool InDebugMode (void) { return m_bDebugMode; }
		inline void InitEntityResolver (CExtension *pExtension, CEntityResolverList *retResolver) { m_Extensions.InitEntityResolver(pExtension, (InDebugMode() ? CExtensionCollection::FLAG_DEBUG_MODE : 0), retResolver); }
		inline bool InResurrectMode (void) { return m_bResurrectMode; }
		bool IsGlobalResurrectPending (CDesignType **retpType);
		inline bool IsRegistered (void) { return m_bRegistered; }
		bool IsStatsPostingEnabled (void);
		inline void NotifyOnObjDeselected (CSpaceObject *pObj) { pObj->FireOnDeselected(); }
		void NotifyOnObjDestroyed (SDestroyCtx &Ctx);
		inline void NotifyOnObjSelected (CSpaceObject *pObj) { pObj->FireOnSelected(); }
		inline ALERROR LoadNewExtension (const CString &sFilespec, const CIntegerIP &FileDigest, CString *retsError) { return m_Extensions.LoadNewExtension(sFilespec, FileDigest, retsError); }
		inline bool LogImageLoad (void) const { return (m_iLogImageLoad == 0); }
		inline void LogOutput (const CString &sLine) const { m_pHost->LogOutput(sLine); }
		void PlaySound (CSpaceObject *pSource, int iChannel);
		void PutPlayerInSystem (CShip *pPlayerShip, const CVector &vPos, CSystemEventList &SavedEvents);
		void RefreshCurrentMission (void);
		inline void RegisterForNotifications (INotifications *pSubscriber) { m_Subscribers.Insert(pSubscriber); }
		ALERROR Reinit (void);
		inline CSpaceObject *RemoveAscendedObj (DWORD dwObjID) { return m_AscendedObjects.RemoveByID(dwObjID); }
		ALERROR SaveDeviceStorage (void);
		ALERROR SaveToStream (IWriteStream *pStream);
		void SetCurrentSystem (CSystem *pSystem);
		inline void SetDebugMode (bool bDebug = true) { m_bDebugMode = bDebug; }
		bool SetExtensionData (EStorageScopes iScope, DWORD dwExtension, const CString &sAttrib, const CString &sData);
		void SetNewSystem (CSystem *pSystem, CSpaceObject *pPOV);
		void SetPOV (CSpaceObject *pPOV);
		void SetPlayerShip (CSpaceObject *pPlayer);
		inline void SetRegistered (bool bRegistered = true) { m_bRegistered = bRegistered; }
		inline void SetRegisteredExtensions (const CMultiverseCollection &Catalog, TArray<CMultiverseCatalogEntry *> *retNotFound) { m_Extensions.SetRegisteredExtensions(Catalog, retNotFound); }
		inline void SetResurrectMode (bool bResurrect = true) { m_bResurrectMode = bResurrect; }
		inline void SetSound (bool bSound = true) { m_bNoSound = !bSound; }
		inline void SetSoundMgr (CSoundMgr *pSoundMgr) { m_pSoundMgr = pSoundMgr; }
		void StartGameTime (void);
		CTimeSpan StopGameTime (void);
		inline void UnregisterForNotifications (INotifications *pSubscriber) { m_Subscribers.DeleteValue(pSubscriber); }
		static CString ValidatePlayerName (const CString &sName);

		CArmorClass *FindArmor (DWORD dwUNID);
		inline CCompositeImageType *FindCompositeImageType (DWORD dwUNID) { return CCompositeImageType::AsType(m_Design.FindEntry(dwUNID)); }
		CEffectCreator *FindDefaultFireEffect (DamageTypes iDamage);
		CEffectCreator *FindDefaultHitEffect (DamageTypes iDamage);
		inline CDesignType *FindDesignType (DWORD dwUNID) { return m_Design.FindEntry(dwUNID); }
		CDeviceClass *FindDeviceClass (DWORD dwUNID);
		inline CEconomyType *FindEconomyType (const CString &sName) { return m_Design.FindEconomyType(sName); }
		inline CEffectCreator *FindEffectType (DWORD dwUNID) { return CEffectCreator::AsType(m_Design.FindEntry(dwUNID)); }
		inline CShipTable *FindEncounterTable (DWORD dwUNID) { return CShipTable::AsType(m_Design.FindEntry(dwUNID)); }
		bool FindExtension (DWORD dwUNID, DWORD dwRelease, CExtension **retpExtension = NULL) { return m_Extensions.FindBestExtension(dwUNID, dwRelease, (InDebugMode() ? CExtensionCollection::FLAG_DEBUG_MODE : 0), retpExtension); }
		inline CGenericType *FindGenericType (DWORD dwUNID) { return CGenericType::AsType(m_Design.FindEntry(dwUNID)); }
		inline CItemTable *FindItemTable (DWORD dwUNID) { return CItemTable::AsType(m_Design.FindEntry(dwUNID)); }
		inline CItemType *FindItemType (DWORD dwUNID) { return CItemType::AsType(m_Design.FindEntry(dwUNID)); }
		inline CMissionType *FindMissionType (DWORD dwUNID) { return CMissionType::AsType(m_Design.FindEntry(dwUNID)); }
		inline COverlayType *FindOverlayType(DWORD dwUNID) { return COverlayType::AsType(m_Design.FindEntry(dwUNID)); }
		inline CPower *FindPower (DWORD dwUNID) { return CPower::AsType(m_Design.FindEntry(dwUNID)); }
		inline CDockScreenType *FindSharedDockScreen (DWORD dwUNID) { return CDockScreenType::AsType(m_Design.FindEntry(dwUNID)); }
		inline CShipClass *FindShipClass (DWORD dwUNID) { return CShipClass::AsType(m_Design.FindEntry(dwUNID)); }
		CShipClass *FindShipClassByName (const CString &sName);
		inline COverlayType *FindShipEnergyFieldType(DWORD dwUNID) { return COverlayType::AsType(m_Design.FindEntry(dwUNID)); }
		inline int FindSound (DWORD dwUNID) { CSoundResource *pSound = FindSoundResource(dwUNID); return (pSound ? pSound->GetSound() : -1); }
		inline CMusicResource *FindMusicResource (DWORD dwUNID) { return CMusicResource::AsType(m_Design.FindEntry(dwUNID)); }
		inline CSoundResource *FindSoundResource (DWORD dwUNID) { return CSoundResource::AsType(m_Design.FindEntry(dwUNID)); }
		inline CSovereign *FindSovereign (DWORD dwUNID) const { return CSovereign::AsType(m_Design.FindEntry(dwUNID)); }
		inline CSpaceEnvironmentType *FindSpaceEnvironment (DWORD dwUNID) { return CSpaceEnvironmentType::AsType(m_Design.FindEntry(dwUNID)); }
		inline CStationType *FindStationType (DWORD dwUNID) { return CStationType::AsType(m_Design.FindEntry(dwUNID)); }
		inline CXMLElement *FindSystemFragment (const CString &sName, CSystemTable **retpTable = NULL) { return m_Design.FindSystemFragment(sName, retpTable); }
		inline CSystemType *FindSystemType (DWORD dwUNID) { return CSystemType::AsType(m_Design.FindEntry(dwUNID)); }
		inline CTopologyNode *FindTopologyNode (const CString &sName) { return m_Topology.FindTopologyNode(sName); }
		CWeaponFireDesc *FindWeaponFireDesc (const CString &sName);
		inline CCodeChain &GetCC (void) { return m_CC; }
		inline CTopologyNode *GetCurrentTopologyNode (void) { return (m_pCurrentSystem ? m_pCurrentSystem->GetTopology() : NULL); }
		inline CSystem *GetCurrentSystem (void) { return m_pCurrentSystem; }
		inline int GetPaintTick (void) { return m_iPaintTick; }
		inline CSpaceObject *GetPOV (void) const { return m_pPOV; }
		inline IPlayerController *GetPlayer (void) const { return m_pPlayer; }
		GenomeTypes GetPlayerGenome (void) const;
		CString GetPlayerName (void) const;
		inline CSpaceObject *GetPlayerShip (void) const { return m_pPlayerShip; }
		CSovereign *GetPlayerSovereign (void) const;
		inline int GetTicks (void) { return m_iTick; }

		inline void ClearLibraryBitmapMarks (void) { m_Design.ClearImageMarks(); m_DynamicImageLibrary.ClearMarks();  }
		void GarbageCollectLibraryBitmaps (void);
		inline CObjectImage *FindLibraryImage (DWORD dwUNID) { return CObjectImage::AsType(m_Design.FindEntry(dwUNID)); }
		inline CG32bitImage *GetLibraryBitmap (DWORD dwUNID, DWORD dwFlags = 0) { return m_Design.GetImage(dwUNID, dwFlags); }
		inline CG32bitImage *GetLibraryBitmapCopy (DWORD dwUNID) { return m_Design.GetImage(dwUNID, CDesignCollection::FLAG_IMAGE_COPY); }
		inline void MarkLibraryBitmaps (void) { m_Design.MarkGlobalImages(); if (m_pCurrentSystem) m_pCurrentSystem->MarkImages(); }
		inline void SweepLibraryBitmaps (void) { m_Extensions.SweepImages(); m_Design.SweepImages(); m_DynamicImageLibrary.Sweep();  }

		inline CDesignCollection &GetDesignCollection (void) { return m_Design; }
		inline CDesignType *GetDesignType (int iIndex) { return m_Design.GetEntry(iIndex); }
		inline int GetDesignTypeCount (void) { return m_Design.GetCount(); }
		inline const CExtension *GetExtensionDesc (int iIndex) { return m_Design.GetExtension(iIndex); }
		inline int GetExtensionDescCount (void) { return m_Design.GetExtensionCount(); }
		inline CItemType *GetItemType (int iIndex) { return (CItemType *)m_Design.GetEntry(designItemType, iIndex); }
		inline int GetItemTypeCount (void) { return m_Design.GetCount(designItemType); }
		inline CPower *GetPower (int iIndex) { return (CPower *)m_Design.GetEntry(designPower, iIndex); }
		inline int GetPowerCount (void) { return m_Design.GetCount(designPower); }
		inline CShipClass *GetShipClass (int iIndex) { return (CShipClass *)m_Design.GetEntry(designShipClass, iIndex); }
		inline int GetShipClassCount (void) { return m_Design.GetCount(designShipClass); }
		inline CMusicResource *GetMusicResource (int iIndex) const { return (CMusicResource *)m_Design.GetEntry(designMusic, iIndex); }
		inline int GetMusicResourceCount (void) const { return m_Design.GetCount(designMusic); }
		inline CSovereign *GetSovereign (int iIndex) const { return (CSovereign *)m_Design.GetEntry(designSovereign, iIndex); }
		inline int GetSovereignCount (void) { return m_Design.GetCount(designSovereign); }
		inline CStationType *GetStationType (int iIndex) { return (CStationType *)m_Design.GetEntry(designStationType, iIndex); }
		inline int GetStationTypeCount (void) { return m_Design.GetCount(designStationType); }
		inline CTopology &GetTopology (void) { return m_Topology; }
		inline CTopologyNode *GetTopologyNode (int iIndex) { return m_Topology.GetTopologyNode(iIndex); }
		inline int GetTopologyNodeCount (void) { return m_Topology.GetTopologyNodeCount(); }

		void PaintObject (CG32bitImage &Dest, const RECT &rcView, CSpaceObject *pObj);
		void PaintObjectMap (CG32bitImage &Dest, const RECT &rcView, CSpaceObject *pObj);
		void PaintPOV (CG32bitImage &Dest, const RECT &rcView, DWORD dwFlags);
		void PaintPOVLRS (CG32bitImage &Dest, const RECT &rcView, Metric rScale, DWORD dwFlags, bool *retbNewEnemies = NULL);
		void PaintPOVMap (CG32bitImage &Dest, const RECT &rcView, Metric rMapScale);
		inline void SetLogImageLoad (bool bLog = true) { CSmartLock Lock(m_cs); m_iLogImageLoad += (bLog ? -1 : +1); }
		void Update (SSystemUpdateCtx &Ctx);
		void UpdateExtended (void);

		void DebugOutput (char *pszLine, ...);

	private:
		struct SLevelEncounter
			{
			CStationType *pType;
			int iWeight;
			CSovereign *pBaseSovereign;
			IShipGenerator *pTable;
			};

		struct STransSystemObject
			{
			CSpaceObject *pObj;
			CTopologyNode *pDest;
			int iArrivalTime;

			STransSystemObject *pNext;
			};

		bool FindByUNID (CIDTable &Table, DWORD dwUNID, CObject **retpObj = NULL);
		CObject *FindByUNID (CIDTable &Table, DWORD dwUNID);
		ALERROR InitCodeChain (const TArray<SPrimitiveDefTable> &CCPrimitives);
		ALERROR InitCodeChainPrimitives (void);
		void InitDefaultEffects (void);
		ALERROR InitDeviceStorage (CString *retsError);
		ALERROR InitFonts (void);
		ALERROR InitLevelEncounterTables (void);
		ALERROR InitRequiredEncounters (CString *retsError);
		ALERROR InitTopology (DWORD dwStartingMap, CString *retsError);
		void NotifyMissionsOfNewSystem (CSystem *pSystem);
		inline void SetCurrentAdventureDesc (CAdventureDesc *pAdventure) { m_pAdventure = pAdventure; }
		void SetHost (IHost *pHost);
		void SetPlayer (IPlayerController *pPlayer);
		void UpdateMissions (int iTick, CSystem *pSystem);
		void UpdateSovereigns (int iTick, CSystem *pSystem);

		//	Design data

		CExtensionCollection m_Extensions;		//	Loaded extensions
		CDesignCollection m_Design;				//	Design collection
		CDeviceStorage m_DeviceStorage;			//	Local cross-game storage

		CString m_sResourceDb;					//	Resource database

		TArray<TArray<SLevelEncounter>> m_LevelEncounterTables;	//	Array of SLevelEncounter arrays
		bool m_bBasicInit;						//	TRUE if we've initialized CodeChain, etc.

		//	Game instance data

		bool m_bRegistered;						//	If TRUE, this is a registered game
		bool m_bResurrectMode;					//	If TRUE, this session is a game resurrect
		int m_iTick;							//	Ticks since beginning of time
		int m_iPaintTick;						//	Advances only when we paint a frame
		CGameTimeKeeper m_Time;					//	Game time tracker
		CAdventureDesc *m_pAdventure;			//	Current adventure
		CSpaceObject *m_pPOV;					//	Point of view
		IPlayerController *m_pPlayer;			//	Player controller
		CSpaceObject *m_pPlayerShip;			//	Player ship
		CSystem *m_pCurrentSystem;				//	Current star system (used by code)
		TSortMap<DWORD, CSystem *> m_StarSystems;	//	Array of CSystem (indexed by ID)
		CTimeDate m_StartTime;					//	Time when we started the game
		DWORD m_dwNextID;						//	Next universal ID
		CTopology m_Topology;					//	Array of CTopologyNode
		CAscendedObjectList m_AscendedObjects;	//	Ascended objects (objects not in any system)
		CMissionList m_AllMissions;				//	List of all missions
		CSystemEventList m_Events;				//	List of all global events
		CObjectTracker m_Objects;				//	Objects across all systems
		CObjectStats m_ObjStats;				//	Object stats (across all systems)

		//	Support structures

		CCriticalSection m_cs;
		IHost *m_pHost;
		CCodeChain m_CC;
		ICCItem *m_pSavedGlobalSymbols;
		CSoundMgr *m_pSoundMgr;
		const CG16bitFont *m_FontTable[fontCount];
		CG16bitFont m_DefaultFonts[fontCount];
		TArray<INotifications *> m_Subscribers;
		CSFXOptions m_SFXOptions;
		CFractalTextureLibrary m_FractalTextureLibrary;
		CGImageCache m_DynamicImageLibrary;
		SViewportAnnotations m_ViewportAnnotations;

		//	Debugging structures

		bool m_bDebugMode;
		bool m_bNoSound;
		int m_iLogImageLoad;					//	If >0 we disable image load logging
	};

//	String-Constant Helpers

Abilities AbilityDecode (const CString &sString);
AbilityModifications AbilityModificationDecode (const CString &sString);
AbilityModificationOptions AbilityModificationOptionDecode (const CString &sString);
AbilityStatus AbilityStatusDecode (const CString &sString);

CString AbilityEncode (Abilities iValue);
CString AbilityModificationEncode (AbilityModifications iValue);
CString AbilityModificationOptionEncode (AbilityModificationOptions iValue);
CString AbilityStatusEncode (AbilityStatus iValue);

CString GetDamageResultsName (EDamageResults iResult);

DestructionTypes GetDestructionCause (const CString &sString);
CString GetDestructionName (DestructionTypes iCause);

CString GetGenomeID (GenomeTypes iGenome);
CString GetGenomeName (GenomeTypes iGenome);
GenomeTypes ParseGenomeID (const CString &sText);

CString GetPaintLayerID (CSystem::LayerEnum iPaintLayer);
CSystem::LayerEnum ParsePaintLayerID (const CString &sID);

MessageTypes GetMessageFromID (const CString &sID);
CString GetMessageID (MessageTypes iMessage);

CSpaceObject::InterSystemResults GetInterSystemResult (const CString &sString);

DWORD GetOrderFlags (IShipController::OrderTypes iOrder);
CString GetOrderName (IShipController::OrderTypes iOrder);
IShipController::OrderTypes GetOrderType (const CString &sString);
int OrderGetDataCount (IShipController::OrderTypes iOrder);
bool OrderHasDataString (IShipController::OrderTypes iOrder);
bool OrderHasDataVector (IShipController::OrderTypes iOrder);
bool OrderHasTarget (IShipController::OrderTypes iOrder, bool *retbRequired = NULL);
bool ParseOrderString (const CString &sValue, IShipController::OrderTypes *retiOrder, IShipController::SData *retData = NULL);

EStorageScopes ParseStorageScopeID (const CString &sID);

//	Miscellaneous Helpers

void AppendReferenceString (CString *iosReference, const CString &sString);
int CalcEffectiveHP (int iLevel, int iHP, int *iHPbyDamageType);
Metric CalcLevelDiffStrength (int iDiff);
ICCItem *CreateItemFromDamageEffects (CCodeChain &CC, SDamageCtx &Ctx);
int GetDamageTypeLevel (DamageTypes iDamage);
Metric *GetDestinyToBellCurveArray (void);
void LoadDamageEffectsFromItem (ICCItem *pItem, SDamageCtx &Ctx);
EManeuverTypes CalcTurnManeuver (int iDesired, int iCurrent, int iRotationAngle);
CString ParseCriteriaParam (char **ioPos, bool bExpectColon = true, bool *retbBinaryParam = NULL);
bool ParseCriteriaParamLevelRange (char **ioPos, int *retiLow, int *retiHigh);
Metric ParseDistance (const CString &sValue, Metric rDefaultScale);

//	CodeChain helper functions (CCUtil.cpp)

#define CCUTIL_FLAG_CHECK_DESTROYED				0x00000001

Metric CalcRandomMetric (CCodeChain &CC, ICCItem *pItem);
ICCItem *CreateDamageSource (CCodeChain &CC, const CDamageSource &Source);
CString CreateDataFieldFromItemList (const TArray<CItem> &List);
CString CreateDataFromItem (CCodeChain &CC, ICCItem *pItem);
ICCItem *CreateDisposition (CCodeChain &CC, CSovereign::Disposition iDisp);
ICCItem *CreateListFromImage (CCodeChain &CC, const CObjectImageArray &Image, int iRotation = 0);
ICCItem *CreateListFromItem (CCodeChain &CC, const CItem &Item);
ICCItem *CreateListFromOrbit (CCodeChain &CC, const COrbit &OrbitDesc);
ICCItem *CreateListFromVector (CCodeChain &CC, const CVector &vVector);
ICCItem *CreatePowerResult (CCodeChain &CC, double rPowerInKW);
CItem CreateItemFromList (CCodeChain &CC, ICCItem *pList);
CSpaceObject *CreateObjFromItem (CCodeChain &CC, ICCItem *pItem, DWORD dwFlags = 0);
ICCItem *CreateObjPointer (CCodeChain &CC, CSpaceObject *pObj);
bool CreateOrbitFromList (CCodeChain &CC, ICCItem *pList, COrbit *retOrbitDesc);
ICCItem *CreateResultFromDataField (CCodeChain &CC, const CString &sValue);
CShip *CreateShipObjFromItem (CCodeChain &CC, ICCItem *pArg);
CStation *CreateStationObjFromItem (CCodeChain &CC, ICCItem *pArg);
CVector CreateVectorFromList (CCodeChain &CC, ICCItem *pList);
CCXMLWrapper *CreateXMLElementFromItem (CCodeChain &CC, ICCItem *pItem);
void DefineGlobalItem (CCodeChain &CC, const CString &sVar, const CItem &Item);
void DefineGlobalSpaceObject (CCodeChain &CC, const CString &sVar, CSpaceObject *pObj);
void DefineGlobalVector (CCodeChain &CC, const CString &sVar, const CVector &vVector);
void DefineGlobalWeaponType (CCodeChain &CC, const CString &sVar, CItemType *pWeaponType);
CInstalledArmor *GetArmorSectionArg (CCodeChain &CC, ICCItem *pArg, CSpaceObject *pObj);
CDamageSource GetDamageSourceArg (CCodeChain &CC, ICCItem *pArg);
DamageTypes GetDamageTypeFromArg (CCodeChain &CC, ICCItem *pArg);
CInstalledDevice *GetDeviceFromItem (CCodeChain &CC, CSpaceObject *pObj, ICCItem *pArg);
CEconomyType *GetEconomyTypeFromItem (CCodeChain &CC, ICCItem *pItem);
CEconomyType *GetEconomyTypeFromString (const CString &sCurrency);
ALERROR GetEconomyUNIDOrDefault (CCodeChain &CC, ICCItem *pItem, DWORD *retdwUNID);
void GetImageDescFromList (CCodeChain &CC, ICCItem *pList, CG32bitImage **retpBitmap, RECT *retrcRect);
ICCItem *GetImageDescProperty (CCodeChain &CC, ICCItem *pImageDesc, const CString &sProperty);
CItem GetItemFromArg (CCodeChain &CC, ICCItem *pArg);
CItemType *GetItemTypeFromArg (CCodeChain &CC, ICCItem *pArg);
bool GetLinkedFireOptions (ICCItem *pArg, DWORD *retdwOptions, CString *retsError);
ALERROR GetPosOrObject (CEvalContext *pEvalCtx, ICCItem *pArg, CVector *retvPos, CSpaceObject **retpObj = NULL, int *retiLocID = NULL);
CWeaponFireDesc *GetWeaponFireDescArg (CCodeChain &CC, ICCItem *pArg);
bool IsVectorItem (ICCItem *pItem);
ALERROR LoadCodeBlock (const CString &sCode, ICCItem **retpCode, CString *retsError = NULL);
ICCItem *StdErrorNoSystem (CCodeChain &CC);

//	Inlines

inline CSpaceObject *CInstalledDevice::GetTarget (CSpaceObject *pSource) const { return ((m_dwTargetID && pSource) ? pSource->GetSystem()->FindObject(m_dwTargetID) : NULL); }
inline void CInstalledDevice::SetTarget (CSpaceObject *pObj) { m_dwTargetID = (pObj ? pObj->GetID() : 0); }

inline bool CItem::IsDisrupted (void) const { return (m_pExtra ? (m_pExtra->m_dwDisruptedTime >= (DWORD)g_pUniverse->GetTicks()) : false); }

inline void CSpaceObjectGrid::AddObject (CSpaceObject *pObj) { GetList(pObj->GetPos()).FastAdd(pObj); }

inline int CalcHPDamageAdj (int iHP, int iDamageAdj)
	{ return (iDamageAdj == 0 ? -1 : (int)((iHP * 100.0 / iDamageAdj) + 0.5)); }
