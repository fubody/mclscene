// Copyright 2014 Matthew Overby.
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

#ifndef MCLSCENE_METATYPES_H
#define MCLSCENE_METATYPES_H 1

#include "TetMesh.hpp"
#include "TriMesh.h"
#include "bsphere.h"
#include "XForm.h"
#include "TriMesh_algo.h"
#include "pugixml.hpp"
#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include "Object.hpp"

///
///	Metadata structs loaded from the config file
///
namespace mcl {

// Nice utility functions for parsing
namespace parse {

	static std::string to_lower( std::string s ){ std::transform( s.begin(), s.end(), s.begin(), ::tolower ); return s; }

	// Returns directory to a file
	static std::string fileDir( std::string fname ){
		size_t pos = fname.find_last_of('/');
		return (std::string::npos == pos) ? "" : fname.substr(0, pos)+'/';
	}
};


//
//	A parameter parsed from the scene file, stored as a string.
//	Has casting functions for convenience, but assumes the type
//	has an overloaded stream operator (with exception of trimesh::vec).
//	I'm really just copying what pugixml does.
//
class Param {
public:
	Param( std::string n, std::string v, std::string t ) : name(n), value(v), type(t) {}
	double as_double() const;
	char as_char() const;
	std::string as_string() const;
	int as_int() const;
	long as_long() const;
	bool as_bool() const;
	float as_float() const;
	trimesh::vec as_vec3() const;

	// Stores the parsed data
	std::string name;
	std::string value; // string value
	std::string type; // string type

	// Some useful vec3 functions:
	void normalize();
	void fix_color(); // if 0-255, sets 0-1
};


//
//	Base
//
class BaseMeta {
public:
	virtual ~BaseMeta(){}
	std::string name;

	// Load parameters and store in the vector/maps.
	virtual bool load_params( const pugi::xml_node &curr_node );

	// Check params is called after an object is parsed to set
	// member data for the derived types.
	virtual bool check_params() { return true; }

	// Returns a parameter with the given tag so long as the parameter
	// is unique (exists only once in the meta). Otherwise it returns
	// the last meta to be added.
	// E.g. <mass type="int" value="1" /> would be
	// int mass = myMeta[mass].as_int()
	virtual Param operator[]( const std::string tag ) const;

	// Adds a parameter to the meta object
	void add_param( const Param &p );

	// Map of parameter indices (in param_vec) with unique names
	std::unordered_map< std::string, int > param_map;

	// Vector of all parameters in the order they were parsed.
	std::vector< Param > param_vec;

	// Transforms are unique in which they are constructed
	// as they are parsed so that order is preserved.
	// The value portion is always a vec3.
	// Their xml syntax is:
	// 	<XForm type="scale/translate/rotate" value="0 100 0" />
	trimesh::xform x_form;	
};


//
//	Camera
//
class CameraMeta : public BaseMeta {
public:
	bool check_params();

	// Set by check_params:
	std::string type;
	trimesh::vec pos, dir, lookat;
};


//
//	Material
//
class MaterialMeta : public BaseMeta {
public:
	bool check_params();

	// Set by check_params:
	std::string type;
	trimesh::vec diffuse, specular;
	float exponent; // i.e. shininess
};


//
//	Light
//
// TODO: Area light
class LightMeta : public BaseMeta {
public:
	bool check_params();

	// Set by check_params:
	std::string type; // point or directional
	trimesh::vec pos, intensity, dir;
};


//
//	Object
//
//	ObjectMeta is a little special compared to the others as
//	it can build and create certain objects. When built, this
//	data is cached and return on subsequent build calls
//
class ObjectMeta : public BaseMeta {
public:
	ObjectMeta() : built_TriMesh(NULL), built_TetMesh(NULL) {}

	bool check_params();

	// Set by check_params:
	std::string type;
	std::string material; // for material_map

	// Build functions
	std::shared_ptr<trimesh::TriMesh> as_TriMesh();
	std::shared_ptr<TetMesh> as_TetMesh();

protected:
	std::shared_ptr<trimesh::TriMesh> built_TriMesh;
	std::shared_ptr<TetMesh> built_TetMesh;
	std::shared_ptr<BaseObject> built_obj;

	#if MCLSCENE_SCENEMANAGER_H
	friend class SceneManager;
	#endif
};



} // end namespace mcl

#endif
