#ifndef DUNEVOXEL_H
#define DUNEVOXEL_H

#include <SDL/SDL.h>

#include "DuneR.h"
#include "DuneGfx.h"

template <int WIDTH, int SAMPLE, int SAMPLELINECOUNT, int DIVGROUP>
struct VoxelDataSize
{
	enum {
		SIZE
		= WIDTH >> ((SAMPLELINECOUNT - 1 - SAMPLE) / DIVGROUP)
	};
};

template <int WIDTH, int SAMPLE, int SAMPLELINECOUNT, int DIVGROUP>
struct VoxelDataGlobalSize
{
	enum {
		SIZE
		= VoxelDataGlobalSize<WIDTH, SAMPLE - 1, SAMPLELINECOUNT, DIVGROUP>::SIZE
		+ VoxelDataSize<WIDTH, SAMPLE, SAMPLELINECOUNT, DIVGROUP>::SIZE
	};
};

template <int WIDTH, int SAMPLELINECOUNT, int DIVGROUP>
struct VoxelDataGlobalSize<WIDTH, 0, SAMPLELINECOUNT, DIVGROUP>
{
	enum {
		SIZE
		= VoxelDataSize<WIDTH, 0, SAMPLELINECOUNT, DIVGROUP>::SIZE
	};
};

class CDuneVoxel
{
	
public:

	typedef Uint8 tColorIndex;
	typedef Sint16 tHeight;

	struct CamInfo
	{
		tHeight ComputeScreenY(Uint32 _iSampleH, Uint32 _iZ) const
		{
			Sint32 iSH = _iSampleH;
			Sint32 iZ = _iZ;

			Sint32 iCamH = 300;
			Sint32 iDeltaH = iSH - iCamH;
			Sint32 iScreenH = 3 * iDeltaH / iZ + Consts::BackH / 2;
			
			return iScreenH;
		}
		
		void GetSampleCorners(const Sint32 _iBase, Sint32* _iXEye, Sint32* _iYEye, Sint32* _iX0, Sint32* _iY0, Sint32* _iX1, Sint32* _iY1) const
		{
			Sint32 iBaseXEye = 0;
			Sint32 iBaseYEye = 0;
			Sint32 iBaseX0 = 0;
			Sint32 iBaseY0 = 400;
			Sint32 iBaseX1 = 400;
			Sint32 iBaseY1 = 0;

			Sint32 iDelta = 2048 * Tick;
			*_iXEye = (iBaseXEye << _iBase) + iDelta;
			*_iYEye = (iBaseYEye << _iBase) + iDelta;
			*_iX0 = (iBaseX0 << _iBase) + iDelta;
			*_iY0 = (iBaseY0 << _iBase) + iDelta;
			*_iX1 = (iBaseX1 << _iBase) + iDelta;
			*_iY1 = (iBaseY1 << _iBase) + iDelta;
		}
		
		Sint32 Tick;
	};

	template <int WIDTH, int HEIGHT, int SAMPLELINECOUNT, int DIVGROUP>
	class Renderer
	{

	public:
	
		enum { TOTAL_SIZE = SAMPLELINECOUNT + VoxelDataGlobalSize<WIDTH, SAMPLELINECOUNT - 1, SAMPLELINECOUNT, DIVGROUP>::SIZE };

		static inline Uint32 SampleCount(Uint32 _iSampleLineIndex)
		{
			return WIDTH >> ((SAMPLELINECOUNT - 1 - _iSampleLineIndex) / DIVGROUP);
		}
		
		Renderer()
		{
			printf("voxel init\n");

			Uint32 iTotalCount = 0;
			for (Uint32 i = 0; i < SAMPLELINECOUNT; ++i)
			{
				Uint32 iCount = SampleCount(i) + 1;

				m_aiSampleHeights[i] = m_aiDataHeights + iTotalCount;
				m_aiSampleColors[i] = m_aiDataColors + iTotalCount;

				printf("\tline %i -> %i samples\n", i, iCount);
				iTotalCount += iCount;
			}

			for (Uint32 i = 0; i < iTotalCount; ++i)
			{
				m_aiDataHeights[i] = 0;
				m_aiDataColors[i] = 0;
			}

			printf("done (%i total, %i expected)\n", iTotalCount, TOTAL_SIZE);
		}

		~Renderer()
		{
		}
		
		void InitGroundPalette(SDL_PixelFormat* _pkFmt, Sint32 _iR0, Sint32 _iG0, Sint32 _iB0, Sint32 _iR1, Sint32 _iG1, Sint32 _iB1)
		{
			Gfx::RampColor(
				m_aiGroundPalette, 0, 256,
				SDL_MapRGBA(_pkFmt, _iR0, _iG0, _iB0, 0xff),
				SDL_MapRGBA(_pkFmt, _iR1, _iG1, _iB1, 0xff),
				_pkFmt
			);
		}
		
		template <class HeightMap>
		void FillSamples(const CamInfo& _kCamInfos, const HeightMap& _kHm)
		{
			static const Sint32 iBase = 16;
			
			Sint32 iXEye, iYEye, iX0, iY0, iX1, iY1;
			_kCamInfos.GetSampleCorners(iBase, &iXEye, &iYEye, &iX0, &iY0, &iX1, &iY1);
			
			Sint32 iSampleDiv = SAMPLELINECOUNT + 1;
			Sint32 iDeltaX0 = (iXEye - iX0) / iSampleDiv;
			Sint32 iDeltaY0 = (iXEye - iY0) / iSampleDiv;
			Sint32 iDeltaX1 = (iXEye - iX1) / iSampleDiv;
			Sint32 iDeltaY1 = (iXEye - iY1) / iSampleDiv;
			
			for (Sint32 i = 0; i < SAMPLELINECOUNT; ++i)
			{
				Uint32 iSampleIndex = SAMPLELINECOUNT - 1 - i;
				Sint32 iSampleCount =  1 + (WIDTH >> (i / DIVGROUP));
				
				Sint32 iDeltaX = (iX1 - iX0) / iSampleCount;
				Sint32 iDeltaY = (iY1 - iY0) / iSampleCount;
				Sint32 iX = iX0 + iDeltaX;
				Sint32 iY = iY0 + iDeltaY;
				
				for (Sint32 j = 0; j < iSampleCount; ++j)
				{
					tHeight iH = 0;
					tColorIndex iC = 0;
					_kHm(iX >> iBase, iY >> iBase, &iH, &iC);

					Sint32 iScreenY = _kCamInfos.ComputeScreenY(iH, 1 + iSampleIndex);
					m_aiSampleHeights[iSampleIndex][j] = iScreenY;
					m_aiSampleColors[iSampleIndex][j] = iC;
					
					iX += iDeltaX;
					iY += iDeltaY;
				}

				iX0 += iDeltaX0;
				iY0 += iDeltaY0;
				iX1 += iDeltaX1;
				iY1 += iDeltaY1;
			}
		}

		void Render(SDL_Surface* _pkDest)
		{
			SDL_LockSurface(_pkDest);
			tColor* piPixels = (tColor*)_pkDest->pixels;

			Uint32 iPixelOffset = (HEIGHT - 1) * WIDTH;

			for (Sint32 i = 0; i < WIDTH; ++i, ++iPixelOffset)
			{
				tColor* piColumn = piPixels + iPixelOffset;
				Sint32 iMaxH = 0;
				Sint32 iCoeff = (i << 16) / WIDTH;
				
				Sint32 iLastColorIndex = m_aiSampleColors[0][(iCoeff * SampleCount(0)) >> 16] << 16;

				for (Uint32 iSample = 0; iSample < SAMPLELINECOUNT; ++iSample)
				{
					Sint32 iDiv = SampleCount(iSample);
					
					tHeight* pHeights = m_aiSampleHeights[iSample];
					tColorIndex* pColors = m_aiSampleColors[iSample];
					
					Sint32 iIndex0 = (iCoeff * iDiv) >> 16;
					Sint32 iIndex1 = iIndex0 + 1;
					
					Sint32 iX0 = (iIndex0 * WIDTH) / iDiv;
					Sint32 iX1 = (iIndex1 * WIDTH) / iDiv;
					Sint32 iRatio = ((i - iX0) << 16) / (iX1 - iX0);

					Sint32 iH0 = pHeights[iIndex0];
					Sint32 iH1 = pHeights[iIndex1];
					Sint32 iH = ((iH0 << 16) + (iH1 - iH0) * iRatio) >> 16;

					Sint32 iC0 = pColors[iIndex0];
					Sint32 iC1 = pColors[iIndex1];
					Sint32 iC = ((iC0 << 16) + (iC1 - iC0) * iRatio) >> 16;

					if (iH > HEIGHT)
						iH = HEIGHT;

					Sint32 iWantedColorIndex = iC << 16;
					
					//tColor iPipo = m_aiGroundPalette[iC];
					
					if (iMaxH < iH)
					{
						Sint32 iDeltaColorIndex = (iWantedColorIndex - iLastColorIndex) / (iH - iMaxH);

						while (iMaxH < iH)
						{
							iLastColorIndex += iDeltaColorIndex;
							*piColumn = m_aiGroundPalette[iLastColorIndex >> 16];
							//*piColumn = iPipo;
							piColumn -= WIDTH;
							++iMaxH;
						}
					}
					
					iLastColorIndex = iWantedColorIndex;
				}
				
				/*
				for (; iMaxH < HEIGHT; ++iMaxH)
				{
					*piColumn = 0xffffffff;
					piColumn -= WIDTH;
				}
				*/
			}

			SDL_UnlockSurface(_pkDest);
		}

	protected:

		tColor m_aiGroundPalette[256];
		
		tColorIndex m_aiBuffer[WIDTH * HEIGHT];

		tHeight* m_aiSampleHeights[SAMPLELINECOUNT];
		tColorIndex* m_aiSampleColors[SAMPLELINECOUNT];

		tHeight m_aiDataHeights[TOTAL_SIZE];
		tColorIndex m_aiDataColors[TOTAL_SIZE];

	};

};

#endif //DUNEVOXEL_H
