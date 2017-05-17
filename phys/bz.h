/**
 * Calculation of first Brillouin zone (3d and approximation in 2d)
 * @author Tobias Weber <tobias.weber@tum.de>
 * @date jun-2014, apr-2017
 * @license GPLv2 or GPLv3
 */

#ifndef __BZ_H__
#define __BZ_H__

#include "../helper/exception.h"
#include "../math/math.h"
#include "../phys/nn.h"
#include "../math/geo.h"
#include "../log/log.h"
#include <vector>

namespace tl {


/**
 * reduce vertices based on nearest neighbours in RLU space (because 1/A space can be non-cubic)
 */
template<class t_vec, class T = typename t_vec::value_type>
static bool reduce_neighbours(
	std::vector<t_vec>& vecNeighbours, std::vector<t_vec>& vecNeighboursHKL,
	const t_vec& vecCentralReflexHKL, T eps)
{
	if(!vecNeighboursHKL.size())
		return true;

	// consider only neighbours and next-neighbours
	auto vecvecNN = get_neighbours<t_vec, std::vector, T>(vecNeighboursHKL, vecCentralReflexHKL, eps);
	if(vecvecNN.size() < 2)
		return false;


	// 1/A
	auto vecNN1 = get_atoms_by_idx<t_vec, std::vector>(vecNeighbours, vecvecNN[0]);
	auto vecNN2 = get_atoms_by_idx<t_vec, std::vector>(vecNeighbours, vecvecNN[1]);

	vecNeighbours = std::move(vecNN1);
	vecNeighbours.insert(vecNeighbours.end(), vecNN2.begin(), vecNN2.end());


	// rlu
	auto vecNN1HKL = get_atoms_by_idx<t_vec, std::vector>(vecNeighboursHKL, vecvecNN[0]);
	auto vecNN2HKL = get_atoms_by_idx<t_vec, std::vector>(vecNeighboursHKL, vecvecNN[1]);

	vecNeighboursHKL = std::move(vecNN1HKL);
	vecNeighboursHKL.insert(vecNeighboursHKL.end(), vecNN2HKL.begin(), vecNN2HKL.end());


	return vecNeighbours.size()!=0;
}


/**
 * 3D Brillouin zones
 */
template<typename T=double>
class Brillouin3D
{
	public:
		template<typename _T> using t_vec = ublas::vector<_T>;

	protected:
		t_vec<T> m_vecCentralReflex;
		t_vec<T> m_vecCentralReflexHKL;

		std::vector<t_vec<T>> m_vecNeighbours;
		std::vector<t_vec<T>> m_vecNeighboursHKL;

		std::vector<t_vec<T>> m_vecVertices;

		std::vector<std::vector<t_vec<T>>> m_vecPolys;
		std::vector<Plane<T>> m_vecPlanes;

		bool m_bValid = 1;
		bool m_bHasCentralPeak = 0;

		T m_eps = 0.001;

	public:
		Brillouin3D() {}
		virtual ~Brillouin3D() {}

		const t_vec<T>& GetCentralReflex() const { return m_vecCentralReflex; }

		const std::vector<t_vec<T>>& GetVertices() const { return m_vecVertices; }
		const std::vector<t_vec<T>>& GetNeighbours() const { return m_vecNeighbours; }

		// m_vecPolys and m_vecPlanes correspond to one another
		const std::vector<std::vector<t_vec<T>>>& GetPolys() const { return m_vecPolys; }
		const std::vector<Plane<T>> GetPlanes() const { return m_vecPlanes; }

		void SetEpsilon(T eps) { m_eps = eps; }

		void Clear()
		{
			m_vecCentralReflex.clear();
			m_vecCentralReflexHKL.clear();
			m_vecNeighbours.clear();
			m_vecNeighboursHKL.clear();
			m_vecVertices.clear();
			m_vecPolys.clear();
			m_vecPlanes.clear();
			m_bValid = 0;
			m_bHasCentralPeak = 0;
		}

		bool IsValid() const { return m_bValid; }

		void SetCentralReflex(const t_vec<T>& vec, const t_vec<T> *pvecHKL=nullptr)
		{
			if(vec.size() != 3)
				throw Err("Brillouin3D needs 3d vectors.");

			m_vecCentralReflex = vec;
			if(pvecHKL)
				m_vecCentralReflexHKL = *pvecHKL;
			m_bHasCentralPeak = 1;
		}

		void AddReflex(const t_vec<T>& vec, const t_vec<T> *pvecHKL=nullptr)
		{
			if(vec.size() != 3)
				throw Err("Brillouin3D needs 3d vectors.");

			m_vecNeighbours.push_back(vec);
			if(pvecHKL)
				m_vecNeighboursHKL.push_back(*pvecHKL);
		}


		/**
		 * calculates the brillouin zone
		 */
		void CalcBZ()
		{
			if(!m_bHasCentralPeak) return;

			// calculate perpendicular planes
			std::vector<Plane<T>> vecMiddlePerps;
			vecMiddlePerps.reserve(m_vecNeighbours.size());

			if(!reduce_neighbours<t_vec<T>, T>(m_vecNeighbours, m_vecNeighboursHKL, m_vecCentralReflexHKL, m_eps))
				return;


			// get middle perpendicular planes
			for(const t_vec<T>& vecN : m_vecNeighbours)
			{
				// line from central reflex to vertex
				Line<T> line(m_vecCentralReflex, vecN-m_vecCentralReflex);

				// middle perpendicular
				Plane<T> planeperp;
				if(!line.GetMiddlePerp(planeperp))
					continue;

				// let normals point outside
				if(!planeperp.GetSide(m_vecCentralReflex))
					planeperp.FlipNormal();
				vecMiddlePerps.emplace_back(std::move(planeperp));
			}


			// get vertices from 3-plane intersections
			for(std::size_t iPlane1=0; iPlane1<vecMiddlePerps.size(); ++iPlane1)
			{
				for(std::size_t iPlane2=iPlane1+1; iPlane2<vecMiddlePerps.size(); ++iPlane2)
				{
					for(std::size_t iPlane3=iPlane2+1; iPlane3<vecMiddlePerps.size(); ++iPlane3)
					{
						const Plane<T>& plane1 = vecMiddlePerps[iPlane1];
						const Plane<T>& plane2 = vecMiddlePerps[iPlane2];
						const Plane<T>& plane3 = vecMiddlePerps[iPlane3];

						t_vec<T> vecVertex;
						if(plane1.intersect(plane2, plane3, vecVertex, m_eps))
						{
							// if duplicate, ignore vertex
							if(std::find_if(m_vecVertices.begin(), m_vecVertices.end(),
								[this, &vecVertex](const t_vec<T>& vec) -> bool
								{ return vec_equal(vecVertex, vec, m_eps); }) != m_vecVertices.end())
								continue;

							m_vecVertices.emplace_back(std::move(vecVertex));
						}
					}
				}
			}


			// get minimum vertex set that is contained within all plane boundaries
			for(auto iterVert = m_vecVertices.begin(); iterVert != m_vecVertices.end();)
			{
				bool bRemoveVertex = 0;

				for(const Plane<T>& plane : vecMiddlePerps)
				{
					if(plane.GetDist(*iterVert) > m_eps)
					{
						bRemoveVertex = 1;
						break;
					}
				}

				if(bRemoveVertex)
				{
					iterVert = m_vecVertices.erase(iterVert);
				}
				else
				{
					set_eps_0(*iterVert);
					++iterVert;
				}
			}


			// calculate polygons by determining which vertex is on which plane
			for(const Plane<T>& plane : vecMiddlePerps)
			{
				std::vector<t_vec<T>> vecPoly;

				for(const t_vec<T>& vecVertex : m_vecVertices)
				{
					if(plane.IsOnPlane(vecVertex, m_eps))
						vecPoly.push_back(vecVertex);
				}

				if(vecPoly.size() >= 3)
				{
					m_vecPolys.emplace_back(std::move(vecPoly));
					m_vecPlanes.push_back(plane);
				}
			}


			// sort vertices in the polygons
			for(std::vector<t_vec<T>>& vecPoly : m_vecPolys)
				sort_poly_verts(vecPoly, m_vecCentralReflex);

			m_bValid = 1;
		}


		/**
		 * Calculates intersection of the BZ with a plane.
		 * @return [ lines, vertices ]
		 */
		std::tuple<std::vector<Line<T>>, std::vector<t_vec<T>>>
		GetIntersection(const Plane<T>& plane) const
		{
			std::vector<Line<T>> vecLines;
			std::vector<t_vec<T>> vecVertices;

			if(m_vecPlanes.size() != m_vecPolys.size())
				return std::make_tuple(vecLines, vecVertices);


			// get all intersection lines
			for(std::size_t i=0; i<m_vecPlanes.size(); ++i)
			{
				Line<T> lineRes;
				if(intersect_plane_poly<t_vec<T>,std::vector,T>(plane, m_vecPlanes[i], m_vecPolys[i], lineRes, m_eps))
					vecLines.emplace_back(std::move(lineRes));
			}


			// calculate line intersection vertices
			for(std::size_t iLine1=0; iLine1<vecLines.size(); ++iLine1)
			{
				for(std::size_t iLine2=iLine1+1; iLine2<vecLines.size(); ++iLine2)
				{
					const Line<T>& line1 = vecLines[iLine1];
					const Line<T>& line2 = vecLines[iLine2];

					T t;
					if(line1.intersect(line2, t, m_eps))
					{
						t_vec<T> vecVert = line1(t);

						// vertex contained within all plane boundaries
						bool bRemoveVertex = 0;
						for(const Plane<T>& planeboundary : m_vecPlanes)
						{
							if(planeboundary.GetDist(vecVert) > m_eps)
							{
								bRemoveVertex = 1;
								break;
							}
						}

						if(!bRemoveVertex)
							vecVertices.emplace_back(std::move(vecVert));
					}
				}
			}

			// sort vertices
			sort_poly_verts(vecVertices);

			return std::make_tuple(vecLines, vecVertices);
		}


		/**
		 * Calculates intersection of the BZ with a line.
		 * @return [ intersection vertices ]
		 */
		std::vector<t_vec<T>>
		GetIntersection(const Line<T>& line) const
		{
			std::vector<t_vec<T>> vecVertices;

			if(m_vecPlanes.size() != m_vecPolys.size())
				return vecVertices;


			// get all intersection lines
			for(std::size_t i=0; i<m_vecPlanes.size(); ++i)
			{
				t_vec<T> vecRes;
				if(intersect_line_poly<t_vec<T>,std::vector,T>(line, m_vecPlanes[i], m_vecPolys[i], vecRes, m_eps))
				{
					set_eps_0(vecRes);
					vecVertices.emplace_back(std::move(vecRes));
				}
			}

			return vecVertices;
		}


		void Print(std::ostream& ostr) const
		{
			ostr << "central: " << m_vecCentralReflex << "\n";

			for(const t_vec<T>& vec : m_vecNeighbours)
				ostr << "vertex: " << vec << "\n";

			ostr.flush();
		}
};



// ----------------------------------------------------------------------------


/**
 * 2D Brillouin zone approximations
 */
template<typename T=double>
class Brillouin2D
{
	public:
		template<typename _T> using t_vec = ublas::vector<_T>;
		template<typename _T> using t_vecpair = std::pair<t_vec<_T>, t_vec<_T> >;
		template<typename _T> using t_vertices = std::vector<t_vecpair<_T> >;

	protected:
		t_vec<T> m_vecCentralReflex;
		t_vec<T> m_vecCentralReflexHKL;

		std::vector<t_vec<T>> m_vecNeighbours;
		std::vector<t_vec<T>> m_vecNeighboursHKL;

		std::vector<t_vecpair<T> > m_vecVertices;
		bool m_bValid = 1;
		bool m_bHasCentralPeak = 0;

		T m_eps = 0.001;

	protected:
		const t_vecpair<T>* GetNextVertexPair(const t_vertices<T>& vecPts, std::size_t *pIdx=0)
		{
			const t_vec<T>& vecLast = m_vecVertices.rbegin()->second;
			for(std::size_t iPt=0; iPt<vecPts.size(); ++iPt)
			{
				const t_vecpair<T>& vecp = vecPts[iPt];

				if(vec_equal(vecp.first, vecLast, m_eps))
				{
					if(pIdx) *pIdx = iPt;
					return &vecp;
				}
			}

			if(vecPts.size())
			{
				log_err("Invalid vertices in Brillouin zone.");
				m_bValid = 0;
			}
			return 0;
		}

	public:
		Brillouin2D() {}
		virtual ~Brillouin2D() {}

		const t_vertices<T>& GetVertices() const { return m_vecVertices; }
		const t_vec<T>& GetCentralReflex() const { return m_vecCentralReflex; }
		const std::vector<t_vec<T>>& GetNeighbours() const { return m_vecNeighbours; }

		void SetEpsilon(T eps) { m_eps = eps; }

		void Clear()
		{
			m_vecCentralReflex.clear();
			m_vecCentralReflexHKL.clear();
			m_vecNeighbours.clear();
			m_vecNeighboursHKL.clear();
			m_vecVertices.clear();
			m_bValid = 0;
			m_bHasCentralPeak = 0;
		}

		bool IsValid() const { return m_bValid; }


		void SetCentralReflex(const t_vec<T>& vec, const t_vec<T> *pvecHKL=nullptr)
		{
			if(vec.size() != 2)
				throw Err("Brillouin2D needs 2d vectors.");

			m_vecCentralReflex = vec;
			if(pvecHKL)
				m_vecCentralReflexHKL = *pvecHKL;
			m_bHasCentralPeak = 1;
		}

		void AddReflex(const t_vec<T>& vec, const t_vec<T> *pvecHKL=nullptr)
		{
			if(vec.size() != 2)
				throw Err("Brillouin2D needs 2d vectors.");

			m_vecNeighbours.push_back(vec);
			if(pvecHKL)
				m_vecNeighboursHKL.push_back(*pvecHKL);
		}


		/**
		 * calculate the brillouin zone approximation
		 */
		void CalcBZ()
		{
			if(!m_bHasCentralPeak) return;

			if(!reduce_neighbours<t_vec<T>, T>(m_vecNeighbours, m_vecNeighboursHKL, m_vecCentralReflexHKL, m_eps))
				return;

			// calculate perpendicular lines
			std::vector<Line<T>> vecMiddlePerps;
			vecMiddlePerps.reserve(m_vecNeighbours.size());

			t_vertices<T> vecPts;

			for(const t_vec<T>& vecN : m_vecNeighbours)
			{
				// line from central reflex to vertex
				Line<T> line(m_vecCentralReflex, vecN-m_vecCentralReflex);

				// middle perpendicular
				Line<T> lineperp;
				if(!line.GetMiddlePerp(lineperp))
					continue;
				vecMiddlePerps.emplace_back(std::move(lineperp));
			}


			// calculate intersections of middle perpendiculars
			for(std::size_t iThisLine=0; iThisLine<vecMiddlePerps.size(); ++iThisLine)
			{
				const Line<T>& lineThis = vecMiddlePerps[iThisLine];
				T tPos = std::numeric_limits<T>::max();
				T tNeg = -tPos;

				for(std::size_t iOtherLine=0; iOtherLine<vecMiddlePerps.size(); ++iOtherLine)
				{
					if(iThisLine == iOtherLine)
						continue;

					T t;
					if(!lineThis.intersect(vecMiddlePerps[iOtherLine], t, m_eps))
						continue;

					if(t>0.)
					{
						if(t < tPos) tPos = t;
					}
					else
					{
						if(t > tNeg) tNeg = t;
					}
				}

				// get closest two intersections of this line with two other lines
				t_vec<T> vecUpper = lineThis(tPos);
				t_vec<T> vecLower = lineThis(tNeg);
				if(vec_equal(vecUpper, vecLower, m_eps))
					continue;

				vecPts.push_back(t_vecpair<T>(vecUpper, vecLower));
			}


			// remove unnecessary vertices
			for(const Line<T>& line : vecMiddlePerps)
			{
				bool bSideReflex = line.GetSide(m_vecCentralReflex);

				for(std::size_t iPt=0; iPt<vecPts.size(); ++iPt)
				{
					t_vec<T>& vecUpper = vecPts[iPt].first;
					t_vec<T>& vecLower = vecPts[iPt].second;

					T tDistUpper=T(0), tDistLower=T(0);
					bool bSideUpper = line.GetSide(vecUpper, &tDistUpper);
					bool bSideLower = line.GetSide(vecLower, &tDistLower);

					if((bSideUpper!=bSideReflex && tDistUpper>m_eps) ||
						(bSideLower!=bSideReflex && tDistLower>m_eps))
					{
						vecPts.erase(vecPts.begin()+iPt);
						--iPt;
						continue;
					}
				}
			}

			if(vecPts.size() == 0)
				return;

			// sort vertices
			m_vecVertices.clear();
			m_vecVertices.reserve(vecPts.size());
			m_vecVertices.push_back(*vecPts.begin());


			vecPts.erase(vecPts.begin());

			std::size_t iIdx;
			while(const t_vecpair<T>* pPair = GetNextVertexPair(vecPts, &iIdx))
			{
				m_vecVertices.push_back(*pPair);
				vecPts.erase(vecPts.begin()+iIdx);
			}

			m_bValid = 1;
		}
};

}

#endif