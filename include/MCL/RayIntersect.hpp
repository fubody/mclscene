// Copyright 2016 Matthew Overby.
// 
// MCLSCENE Uses the BSD 2-Clause License (http://www.opensource.org/licenses/BSD-2-Clause)
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other materials
//    provided with the distribution.
// THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR  A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE UNIVERSITY OF MINNESOTA, DULUTH OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
// IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// By Matt Overby (http://www.mattoverby.net)

#ifndef MCLSCENE_RAYINTERSECT_H
#define MCLSCENE_RAYINTERSECT_H 1

#include <memory>
#include <Vec.h>

namespace mcl {

namespace intersect {

	struct Ray {
		trimesh::vec origin, direction;
	};

	struct Payload {
		Payload(){ t_min=1e-8; t_max=9999999.0; }
		double t_min, t_max;
		trimesh::vec n, hit_point;
		std::string material;
	};

	// ray -> triangle without early exit
	static inline bool ray_triangle( const Ray &ray, const trimesh::vec &p0, const trimesh::vec &p1, const trimesh::vec &p2,
		const trimesh::vec &n0, const trimesh::vec &n1, const trimesh::vec &n2, Payload &payload ){
		using namespace trimesh;

		const vec e0 = p1 - p0;
		const vec e1 = p0 - p2;
		const vec n = e1.cross( e0 );

		const vec e2 = ( 1.0f / n.dot( ray.direction ) ) * ( p0 - ray.origin );
		const vec i  = ray.direction.cross( e2 );

		float beta  = i.dot( e1 );
		float gamma = i.dot( e0 );
		float alpha = 1.f - beta - gamma;

		float t = n.dot( e2 );
		bool hit = ( (t<payload.t_max) & (t>payload.t_min) & (beta>=0.0f) & (gamma>=0.0f) & (beta+gamma<=1) );

		if( hit ){
			payload.n = alpha*n0 + beta*n1 + gamma*n2;
			payload.t_max = t;
			payload.hit_point = ray.origin + ray.direction*t;
			return true;
		}

		return false;

	} // end  ray -> triangle

} // end namespace intersect

} // end namespace mcl

#endif
