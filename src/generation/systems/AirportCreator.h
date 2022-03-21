#pragma once

#include "mgpch.h"
#include "../creator.h"

class AirportCreator: public icreator
{
public:

	using icreator::icreator;

	void initInChunk(bool forMesh, Mesh& mesh) override;


	math::mesh::ShadedMultiPolygon2D polygonsFromElement(const osm::element& elm) override;


	math::mesh::ShadedMultiPolygon2D polygonsFromOSM() override;


	void meshFromElement(Mesh& mesh, const osm::element& elm) override;


	void meshFromOSM(Mesh& mesh) override;

protected:
private:
};
