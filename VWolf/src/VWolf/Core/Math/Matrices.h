#pragma once

#include "VWolf/Core/Base.h"

namespace VWolf {
	struct MatrixFloat4x4 {
		float aa, ab, ac, ad,
			  ba, bb, bc, bd,
			  ca, cb, cc, cd,
			  da, db, dc, dd;

		MatrixFloat4x4(): MatrixFloat4x4(1.0f) {

		}

		MatrixFloat4x4(float diag): MatrixFloat4x4(
			diag, 0.0f, 0.0f, 0.0f,
			0.0f, diag, 0.0f, 0.0f,
			0.0f, 0.0f, diag, 0.0f,
			0.0f, 0.0f, 0.0f, diag) {

		}

		MatrixFloat4x4(
			float  aa, float ab, float ac, float ad,
			float  ba, float bb, float bc, float bd,
			float  ca, float cb, float cc, float cd,
			float  da, float db, float dc, float dd) {
			this->aa = aa;
			this->ab = ab;
			this->ac = ac;
			this->ad = ad;
			this->ba = ba;
			this->bb = bb;
			this->bc = bc;
			this->bd = bd;
			this->ca = ca;
			this->cb = cb;
			this->cc = cc;
			this->cd = cd;
			this->da = da;
			this->db = db;
			this->dc = dc;
			this->dd = dd;
		}

		static MatrixFloat4x4 Identity() {
			return { 1.0f };
		}

		inline std::string ToString() const {

			std::stringstream ss;
			ss << "Matrix 4x4: \n";
			ss << "[aa:" << (*this).aa << ", ab:" << (*this).ab << ", ac:" << (*this).ac << ", ad:" << (*this).ad << "] \n";
			ss << "[ba:" << (*this).ba << ", bb:" << (*this).bb << ", bc:" << (*this).bc << ", bd:" << (*this).bd << "] \n";
			ss << "[ca:" << (*this).ca << ", cb:" << (*this).cb << ", cc:" << (*this).cc << ", cd:" << (*this).cd << "] \n";
			ss << "[da:" << (*this).da << ", db:" << (*this).db << ", dc:" << (*this).dc << ", dd:" << (*this).dd << "]";
			return ss.str();
		}
	};

	inline std::ostream& operator<<(std::ostream& os, const MatrixFloat4x4& v)
	{
		os << "Matrix 4x4: \n";
		os << "[aa:" << v.aa << ", ab:" << v.ab << ", ac:" << v.ac << ", ad:" << v.ad << "] \n";
		os << "[ba:" << v.ba << ", bb:" << v.bb << ", bc:" << v.bc << ", bd:" << v.bd << "] \n";
		os << "[ca:" << v.ca << ", cb:" << v.cb << ", cc:" << v.cc << ", cd:" << v.cd << "] \n";
		os << "[da:" << v.da << ", db:" << v.db << ", dc:" << v.dc << ", dd:" << v.dd << "]";
		return os;
	}

	struct MatrixFloat3x3 {
		float aa, ab, ac,
			ba, bb, bc,
			ca, cb, cc;

		MatrixFloat3x3() : MatrixFloat3x3(1.0f) {

		}

		MatrixFloat3x3(float diag) : MatrixFloat3x3(
			diag, 0.0f, 0.0f, 
			0.0f, diag, 0.0f, 
			0.0f, 0.0f, diag) {

		}

		MatrixFloat3x3(
			float  aa, float ab, float ac,
			float  ba, float bb, float bc,
			float  ca, float cb, float cc) {
			this->aa = aa;
			this->ab = ab;
			this->ac = ac;
			this->ba = ba;
			this->bb = bb;
			this->bc = bc;
			this->ca = ca;
			this->cb = cb;
			this->cc = cc;
		}

		static MatrixFloat3x3 Identity() {
			return { 1.0f };
		}

		inline std::string ToString() const {

			std::stringstream ss;
			ss << "Matrix 3x3: \n";
			ss << "[aa:" << (*this).aa << ", ab:" << (*this).ab << ", ac:" << (*this).ac << "] \n";
			ss << "[ba:" << (*this).ba << ", bb:" << (*this).bb << ", bc:" << (*this).bc << "] \n";
			ss << "[ca:" << (*this).ca << ", cb:" << (*this).cb << ", cc:" << (*this).cc << "]";
			return ss.str();
		}
	};

	inline std::ostream& operator<<(std::ostream& os, const MatrixFloat3x3& v)
	{
		os << "Matrix 4x4: \n";
		os << "Matrix 3x3: \n";
		os << "[aa:" << v.aa << ", ab:" << v.ab << ", ac:" << v.ac << "] \n";
		os << "[ba:" << v.ba << ", bb:" << v.bb << ", bc:" << v.bc << "] \n";
		os << "[ca:" << v.ca << ", cb:" << v.cb << ", cc:" << v.cc << "]";
		return os;
	}

	struct MatrixFloat2x2 {
		float aa, ab,
			ba, bb;

		MatrixFloat2x2() : MatrixFloat2x2(1.0f) {

		}

		MatrixFloat2x2(float diag) : MatrixFloat2x2(
			diag, 0.0f,
			0.0f, diag) {

		}

		MatrixFloat2x2(
			float  aa, float ab,
			float  ba, float bb) {
			this->aa = aa;
			this->ab = ab;
			this->ba = ba;
			this->bb = bb;
		}

		static MatrixFloat2x2 Identity() {
			return { 1.0f };
		}

		inline std::string ToString() const {

			std::stringstream ss;
			ss << "Matrix 2x2: \n";
			ss << "[aa:" << (*this).aa << ", ab:" << (*this).ab << "] \n";
			ss << "[ba:" << (*this).ba << ", bb:" << (*this).bb << "]";
			return ss.str();
		}
	};

	inline std::ostream& operator<<(std::ostream& os, const MatrixFloat2x2& v)
	{
		os << "Matrix 2x2: \n";
		os << "[aa:" << v.aa << ", ab:" << v.ab << "] \n";
		os << "[ba:" << v.ba << ", bb:" << v.bb << "]";
		return os;
	}
}