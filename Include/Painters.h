//	Painters.h
//
//	Transcendence UI Engine
//	Copyright (c) 2010 by George Moromisato. All Rights Reserved.

#pragma once

class CHoverDescriptionPainter
	{
	public:
		CHoverDescriptionPainter (const CVisualPalette &VI);

		inline void Hide (void) { m_cxWidth = 0; }
		inline bool IsVisible (void) const { return (m_cxWidth > 0); }
		void Paint (CG32bitImage &Dest) const;
		inline void SetBackColor (const CG32bitPixel rgbValue) { m_rgbBack = rgbValue; }
		void SetDescription (const CString &sValue);
		inline void SetDescriptionColor (const CG32bitPixel rgbValue) { m_rgbDescription = rgbValue; }
		inline void SetItem (const CItem &Item) { m_Item = Item; Invalidate(); }
		inline void SetTitle (const CString &sValue) { m_sTitle = sValue; m_Item = CItem(); Invalidate(); }
		inline void SetTitleColor (const CG32bitPixel rgbValue) { m_rgbTitle = rgbValue; }
		void Show (int x, int y, int cxWidth, const RECT &rcContainer);

	private:
		void InitRects (int cxWidth, int cyHeight) const;
		inline void Invalidate (void) { m_rcRect = { 0, 0, 0, 0 };	}
		inline bool IsInvalid (void) const { return (m_rcRect.left == 0 && m_rcRect.right == 0); }
		void FormatText (void) const;
		void PaintItem (CG32bitImage &Dest) const;
		void PaintText (CG32bitImage &Dest) const;

		const CVisualPalette &m_VI;

		CString m_sTitle;					//	Title to paint
		CString m_sDescription;				//	Description
		CItem m_Item;						//	Item to show (may be empty)
		CG32bitPixel m_rgbBack;				//	Background color
		CG32bitPixel m_rgbTitle;			//	Title color
		CG32bitPixel m_rgbDescription;		//	Description color

		int m_xPos;							//	Draw the description at this position, but
		int m_yPos;							//		adjust to fit in the container
		int m_cxWidth;						//	Desired width of description box (0 = hidden)
		RECT m_rcContainer;					//	Position description inside this container

		//	Initialized in Format

		mutable RECT m_rcRect;				//	Rect of entire background
		mutable RECT m_rcText;				//	Rect of text area
		mutable CTextBlock m_DescriptionRTF;	//	Rich text to draw
	};
class ITransitionEffectPainter
	{
	public:
		virtual void Paint (CG32bitImage &Dest, const RECT &rcRect) { };
		virtual void Update (void) { };
	};
class CStargateEffectPainter : public ITransitionEffectPainter
	{
	public:
		CStargateEffectPainter (void);

		void Paint (CG32bitImage &Dest, const RECT &rcRect);
		void Update (void);

	private:
		struct SWormholePos
			{
			Metric rAngle;
			Metric rRadius;
			Metric rDepth;
			};

		struct STracer
			{
			STracer (void) : Points(0)
				{ }

			TQueue<SWormholePos> Points;
			SWormholePos Vel;
			CG32bitPixel Color;
			};

		struct SPaintCtx
			{
			CG32bitImage *pDest;
			Metric xCenter;
			Metric yCenter;
			Metric cxHalfWidth;
			Metric cyHalfHeight;
			};

		void InitGradientColorTable (int iRadius);
		void InitTracer (STracer &Tracer);
		void PaintTracer (SPaintCtx &Ctx, const STracer &Tracer);
		void TransformPos(SPaintCtx &Ctx, const SWormholePos &Pos, int *retx, int *rety);

		int m_iInitialUpdates;				//	Number of ticks to update before first paint
		int m_iMaxTracerCount;				//	Max number of tracers
		int m_iMaxTracerLength;				//	Max number of segments per tracer
		int m_iNewTracerCount;				//	Number of new tracers per tick
		BYTE m_byTracerOpacity;				//	Max opacity of tracers
		Metric m_rMaxDepth;					//	Max depth of tracers
		Metric m_rRotationVel;				//	Effect rotation in radians per tick
		Metric m_rGlowAccel;				//	Glow acceleration (pixels per tick)

		TArray<CG32bitPixel> m_GradientColorTable;
		TArray<STracer> m_Tracers;
		Metric m_rRotation;
		Metric m_rGlowVel;
		Metric m_rGlowRadius;
	};
class CAutomataEffectPainter : public ITransitionEffectPainter
	{
	public:
		CAutomataEffectPainter (int width, int height);
		int CountLiveNeighbors (int x, int y);
		void InitGradientColorTable (void);
		bool IsAlive(int x, int y);
		void Paint (CG32bitImage &Dest, const RECT &rcRect);
		void Update (void);

	private:
		struct SCell
			{
			SCell(void) : iOpacity(0), bAlive(false) {}
			SCell(bool alive) : iOpacity(0), bAlive(alive) {}
			BYTE iOpacity;				//	A value from 0 to 255. Increases while the cell is alive and decreases while the cell is dead
			bool bAlive;				//	Whether the cell is alive or dead
			bool bAliveNext;			//	Whether the cell will be alive or dead on the next tick. Used only during Update()
			};

		struct SPaintCtx
			{
			CG32bitImage *pDest;
			Metric xCenter;
			Metric yCenter;
			Metric cxHalfWidth;
			Metric cyHalfHeight;
			};

		int m_iTicks;						//	Number of ticks passed
		int m_iInitialUpdates;				//	Number of ticks to update before first paint
		int m_iOpacityInc;					//	The rate at which a cell changes its opacity
		int m_iWidth;						//	Width of the grid
		int m_iHeight;						//	Height of the grid

		TArray<CG32bitPixel> m_GradientColorTable;
		TArray< TArray<SCell> > m_Grid;
	};