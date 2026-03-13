#include "ZeroTypes.h"
#include "ZeroLine.h"

#include "ZeroCamera.h"
#include "ZeroRenderState.h"
#include "ZeroRenderDescriptor.h"

static ZeroRenderState renderStates[] =
{
	ZeroRenderState (ZERO_RENDERSTATE_ALPHABLENDENABLE, TRUE),
	ZeroRenderState (ZERO_RENDERSTATE_SRCBLEND, ZERO_BLEND_SRCALPHA),
	ZeroRenderState (ZERO_RENDERSTATE_DESTBLEND, ZERO_BLEND_INVSRCALPHA),
	ZeroRenderState (ZERO_RENDERSTATE_FOGENABLE, FALSE),
	ZeroRenderState (ZERO_RENDERSTATE_LIGHTING, FALSE),
	ZeroRenderState (ZERO_RENDERSTATE_END, 0)
};

struct VertexTL
{
	ZeroVector4 vv;
	ZeroColor diffuse;
	ZeroColor specular;
	ZeroUV uv;

	VertexTL()
	{
	}

	VertexTL(const float x, const float y, const float vz, const float rhw,
		const ZeroColor diffuse, const ZeroColor specular, const float u, const float v)
		: vv(x, y, vz, rhw), diffuse(diffuse), specular(specular), uv(u, v)
	{
	}
};

struct ColorVertex
{
	ZeroVector3 vv;
	ZeroColor diffuse;

	ColorVertex()
	{
	}

	ColorVertex(const float x, const float y, const float z, const ZeroColor diffuse)
		: vv(x, y, z), diffuse(diffuse)
	{
	}
};

//
// DRAW A 2D CIRCLE
//
void GraphicCircle (ZeroCamera *camera,
					const ZeroColor color,
					const ZeroPoint<float> &p0,
					const float radius,
					const float z)
{
	// get the camera viewport
	const ZeroViewport *viewport = &camera->GetViewport();
	const float l = (float)(viewport->pane.x0);
	const float r = (float)(viewport->pane.x1-1);
	const float t = (float)(viewport->pane.y0);
	const float b = (float)(viewport->pane.y1-1);

	// if the circle is outside the view...
	if ((p0.x - radius >= r) ||
		(p0.y - radius >= b) ||
		(p0.x + radius <= l) ||
		(p0.y + radius <= t))
	{
		// trivial rejection
		return;
	}

	// project z
	const float rhw = 1.0f / z;
	const float vz = viewport->homScalez0 + viewport->homScalez1 * rhw;

	// create the render descriptor
	ZeroRenderDescriptor desc(camera);
	desc.primitive = ZERO_PRIMITIVE_LINESTRIP;
	desc.format = ZERO_FORMAT_RHW | ZERO_FORMAT_DIFFUSE;
	if ((p0.x - radius <= l) ||
		(p0.y - radius <= t) ||
		(p0.x + radius >= r) ||
		(p0.y + radius >= b))
	{
		desc.flags = ZERO_RENDERFLAG_CLIP;
	}
	desc.vertexCount = 17;
	desc.indexCount = 0;

	// lock the render descriptor
	camera->Lock(desc, true);

	// fill in the vertices
	for (int i = 0; i <= 16; i++)
	{
		const float a (i * PI2 / 16);
		desc.vertexRecord.AddVertexPos(p0.x + cosf(a) * radius, p0.y + sinf(a) * radius, i != 0, vz, rhw);
		desc.vertexRecord.AddVertexColor(color);
	}
	// unlock the render descriptor
	camera->Unlock(desc);

	// rewind pointers
	desc.vertexRecord.RewindPointersBy(desc.vertexCount);

	// set the render state
	camera->SetRenderStates(renderStates);
	camera->SetTextureStages(0, NULL, NULL);

	// set the world matrix
	const ZeroMatrix *matrix = &identityMatrix;
	camera->SetWorldMatrix(1, &matrix);

	// render the geometry
	camera->Render(desc);
}


//
// DRAW A 2D DIAMOND
//
void GraphicDiamond (ZeroCamera *camera,
					 const ZeroColor color,
					 const ZeroPoint<float> &p0,
					 const float radius,
					 const float z)
{
	// get the camera viewport
	const ZeroViewport *viewport = &camera->GetViewport();
	const float l = (float)(viewport->pane.x0);
	const float r = (float)(viewport->pane.x1-1);
	const float t = (float)(viewport->pane.y0);
	const float b = (float)(viewport->pane.y1-1);

	// if the diamond is outside the view...
	if ((p0.x - radius >= r) ||
		(p0.y - radius >= b) ||
		(p0.x + radius <= l) ||
		(p0.y + radius <= t))
	{
		// trivial rejection
		return;
	}

	// project z
	const float rhw = 1.0f / z;
	const float vz = viewport->homScalez0 + viewport->homScalez1 * rhw;

	// create the render descriptor
	ZeroRenderDescriptor desc(camera);
	desc.primitive = ZERO_PRIMITIVE_LINESTRIP;
	desc.format = ZERO_FORMAT_RHW | ZERO_FORMAT_DIFFUSE;
	if ((p0.x - radius <= l) ||
		(p0.y - radius <= t) ||
		(p0.x + radius >= r) ||
		(p0.y + radius >= b))
	{
		desc.flags = ZERO_RENDERFLAG_CLIP;
	}
	desc.vertexCount = 5;
	desc.indexCount = 0;

	// lock the render descriptor
	camera->Lock(desc, true);

	// fill in the vertices

	// right
	desc.vertexRecord.AddVertexPos(p0.x + radius, p0.y, false, vz, rhw);
	desc.vertexRecord.AddVertexColor(color);

	// bottom
	desc.vertexRecord.AddVertexPos(p0.x, p0.y + radius, true, vz, rhw);
	desc.vertexRecord.AddVertexColor(color);

	// left
	desc.vertexRecord.AddVertexPos(p0.x - radius, p0.y, true, vz, rhw);
	desc.vertexRecord.AddVertexColor(color);

	// top
	desc.vertexRecord.AddVertexPos(p0.x, p0.y - radius, true, vz, rhw);
	desc.vertexRecord.AddVertexColor(color);

	// right
	desc.vertexRecord.AddVertexPos(p0.x + radius, p0.y, true, vz, rhw);
	desc.vertexRecord.AddVertexColor(color);

	// unlock the render descriptor
	camera->Unlock(desc);

	// rewind pointers
	desc.vertexRecord.RewindPointersBy(desc.vertexCount);

	// set the render state
	camera->SetRenderStates(renderStates);
	camera->SetTextureStages(0, NULL, NULL);

	// set the world matrix
	const ZeroMatrix *matrix = &identityMatrix;
	camera->SetWorldMatrix(1, &matrix);

	// render the geometry
	camera->Render(desc);
}

//
// DRAW A TWO-COLORED 3D LINE
//
void WorldCLine (ZeroCamera *camera,
				 const ZeroColor color0,
				 const ZeroColor color1,
				 const ZeroVector3 &p0,
				 const ZeroVector3 &p1)
{
	// create the render descriptor
	ZeroRenderDescriptor desc(camera);
	desc.primitive = ZERO_PRIMITIVE_LINESTRIP;
	desc.format = ZERO_FORMAT_DIFFUSE;
	desc.flags = ZERO_RENDERFLAG_CLIP;
	desc.vertexCount = 2;
	desc.indexCount = 0;

#if (PLATFORM == PLATFORM_PS2)
	desc.flags |= ZERO_RENDERFLAG_UNCOMPRESSED;
	// set bounding box
	camera->GetCameraBox(desc.vertexRecord.boxMin, desc.vertexRecord.boxMax);
	desc.vertexRecord.boxScale.x = 0xFFFF / (desc.vertexRecord.boxMax.x - desc.vertexRecord.boxMin.x);
	desc.vertexRecord.boxScale.y = 0xFFFF / (desc.vertexRecord.boxMax.y - desc.vertexRecord.boxMin.y);
	desc.vertexRecord.boxScale.z = 0xFFFF / (desc.vertexRecord.boxMax.z - desc.vertexRecord.boxMin.z);
#endif

	// lock the render descriptor
	camera->Lock(desc, true);

	// start
	desc.vertexRecord.AddVertexPos(p0.x, p0.y, p0.z, false);
	desc.vertexRecord.AddVertexColor(color0);

	// end
	desc.vertexRecord.AddVertexPos(p1.x, p1.y, p1.z, true);
	desc.vertexRecord.AddVertexColor(color1);

	// unlock the render descriptor
	camera->Unlock(desc);

	// rewind pointers
	desc.vertexRecord.RewindPointersBy(desc.vertexCount);

	// set the render state
	camera->SetRenderStates(renderStates);
	camera->SetTextureStages(0, NULL, NULL);

	// set the world matrix
	const ZeroMatrix *matrix = &identityMatrix;
	camera->SetWorldMatrix(1, &matrix);

	// render the geometry
	camera->Render(desc);
}

//
// DRAW A ONE-COLORED 3D LINE
//
void WorldLine (ZeroCamera *camera,
				const ZeroColor color,
				const ZeroVector3 &p0,
				const ZeroVector3 &p1)
{
	// create the render descriptor
	ZeroRenderDescriptor desc(camera);
	desc.primitive = ZERO_PRIMITIVE_LINESTRIP;
	desc.format = ZERO_FORMAT_DIFFUSE;
	desc.flags = ZERO_RENDERFLAG_CLIP;
	desc.vertexCount = 2;
	desc.indexCount = 0;

#if (PLATFORM == PLATFORM_PS2)
	desc.flags |= ZERO_RENDERFLAG_UNCOMPRESSED;
	// set bounding box
	camera->GetCameraBox(desc.vertexRecord.boxMin, desc.vertexRecord.boxMax);
	desc.vertexRecord.boxScale.x = 0xFFFF / (desc.vertexRecord.boxMax.x - desc.vertexRecord.boxMin.x);
	desc.vertexRecord.boxScale.y = 0xFFFF / (desc.vertexRecord.boxMax.y - desc.vertexRecord.boxMin.y);
	desc.vertexRecord.boxScale.z = 0xFFFF / (desc.vertexRecord.boxMax.z - desc.vertexRecord.boxMin.z);
#endif

	// lock the render descriptor
	camera->Lock(desc, true);

	// start
	desc.vertexRecord.AddVertexPos(p0.x, p0.y, p0.z, false);
	desc.vertexRecord.AddVertexColor(color);

	// end
	desc.vertexRecord.AddVertexPos(p1.x, p1.y, p1.z, true);
	desc.vertexRecord.AddVertexColor(color);

	// unlock the render descriptor
	camera->Unlock(desc);

	// rewind pointers
	desc.vertexRecord.RewindPointersBy(desc.vertexCount);

	// set the render state
	camera->SetRenderStates(renderStates);
	camera->SetTextureStages(0, NULL, NULL);

	// set the world matrix
	const ZeroMatrix *matrix = &identityMatrix;
	camera->SetWorldMatrix(1, &matrix);

	// render the geometry
	camera->Render(desc);
}

//
// DRAW A PRE-CLIPPED 2D LINE
//
void ClippedLine(ZeroCamera *camera,
				 const ZeroColor color, 
				 const ZeroPoint<float> &p0,
				 const ZeroPoint<float> &p1,
				 const float z)
{
	// get the camera viewport
	const ZeroViewport *viewport = &camera->GetViewport();

	// project z
	const float rhw = 1.0f / z;
	const float vz = viewport->homScalez0 + viewport->homScalez1 * rhw;

	// create the render descriptor
	ZeroRenderDescriptor desc(camera);
	desc.primitive = ZERO_PRIMITIVE_LINESTRIP;
	desc.format = ZERO_FORMAT_RHW | ZERO_FORMAT_DIFFUSE;
	desc.vertexCount = 2;
	desc.indexCount = 0;

	// lock the render descriptor
	camera->Lock(desc, true);

	// start
	desc.vertexRecord.AddVertexPos(p0.x, p0.y, false, vz, rhw);
	desc.vertexRecord.AddVertexColor(color);

	// end
	desc.vertexRecord.AddVertexPos(p1.x, p1.y, true, vz, rhw);
	desc.vertexRecord.AddVertexColor(color);

	// unlock the render descriptor
	camera->Unlock(desc);

	// rewind pointers
	desc.vertexRecord.RewindPointersBy(desc.vertexCount);

	// set the render state
	camera->SetRenderStates(renderStates);
	camera->SetTextureStages(0, NULL, NULL);

	// set the world matrix
	const ZeroMatrix *matrix = &identityMatrix;
	camera->SetWorldMatrix(1, &matrix);

	// render the geometry
	camera->Render(desc);
}

//
// DRAW A PRE-CLIPPED 2D FILLED RECTANGLE
//
void ClippedRectFilled(ZeroCamera *camera,
					   const ZeroColor color, 
					   const ZeroArea<float> &rect,
					   const float z)
{
	// get the camera viewport
	const ZeroViewport *viewport = &camera->GetViewport();

	// project z
	const float rhw = 1.0f / z;
	const float vz = viewport->homScalez0 + viewport->homScalez1 * rhw;

	// create the render descriptor
	ZeroRenderDescriptor desc(camera);
	desc.primitive = ZERO_PRIMITIVE_TRIANGLESTRIP;
	desc.format = ZERO_FORMAT_RHW | ZERO_FORMAT_DIFFUSE;
	desc.vertexCount = 4;
	desc.indexCount = 0;

	// lock the render descriptor
	camera->Lock(desc, true);

	// upper left
	desc.vertexRecord.AddVertexPos(rect.p0.x, rect.p0.y, false, vz, rhw);
	desc.vertexRecord.AddVertexColor(color);

	// lower left
	desc.vertexRecord.AddVertexPos(rect.p0.x, rect.p1.y, true, vz, rhw);
	desc.vertexRecord.AddVertexColor(color);

	// upper right
	desc.vertexRecord.AddVertexPos(rect.p1.x, rect.p0.y, true, vz, rhw);
	desc.vertexRecord.AddVertexColor(color);

	// lower right
	desc.vertexRecord.AddVertexPos(rect.p1.x, rect.p1.y, true, vz, rhw);
	desc.vertexRecord.AddVertexColor(color);

	// unlock the render descriptor
	camera->Unlock(desc);

	// rewind pointers
	desc.vertexRecord.RewindPointersBy(desc.vertexCount);

	// set the render state
	camera->SetRenderStates(renderStates);
	camera->SetTextureStages(0, NULL, NULL);

	// set the world matrix
	const ZeroMatrix *matrix = &identityMatrix;
	camera->SetWorldMatrix(1, &matrix);

	// render the geometry
	camera->Render(desc);
}

//
// DRAW A PRE-CLIPPED 2D EMPTY RECTANGLE
//
void ClippedRectEmpty(ZeroCamera *camera,
					   const ZeroColor color, 
					   const ZeroArea<float> &rect,
					   const float z)
{
	// get the camera viewport
	const ZeroViewport *viewport = &camera->GetViewport();

	// project z
	const float rhw = 1.0f / z;
	const float vz = viewport->homScalez0 + viewport->homScalez1 * rhw;

	// create the render descriptor
	ZeroRenderDescriptor desc(camera);
	desc.primitive = ZERO_PRIMITIVE_LINESTRIP;
	desc.format = ZERO_FORMAT_RHW | ZERO_FORMAT_DIFFUSE;
	desc.vertexCount = 5;
	desc.indexCount = 0;

	// lock the render descriptor
	camera->Lock(desc, true);

	// upper left
	desc.vertexRecord.AddVertexPos(rect.p0.x, rect.p0.y, false, vz, rhw);
	desc.vertexRecord.AddVertexColor(color);

	// upper right
	desc.vertexRecord.AddVertexPos(rect.p1.x, rect.p0.y, true, vz, rhw);
	desc.vertexRecord.AddVertexColor(color);

	// lower right
	desc.vertexRecord.AddVertexPos(rect.p1.x, rect.p1.y, true, vz, rhw);
	desc.vertexRecord.AddVertexColor(color);

	// lower left
	desc.vertexRecord.AddVertexPos(rect.p0.x, rect.p1.y, true, vz, rhw);
	desc.vertexRecord.AddVertexColor(color);

	// upper left
	desc.vertexRecord.AddVertexPos(rect.p0.x, rect.p0.y, true, vz, rhw);
	desc.vertexRecord.AddVertexColor(color);

	// unlock the render descriptor
	camera->Unlock(desc);

	// rewind pointers
	desc.vertexRecord.RewindPointersBy(desc.vertexCount);

	// set the render state
	camera->SetRenderStates(renderStates);
	camera->SetTextureStages(0, NULL, NULL);

	// set the world matrix
	const ZeroMatrix *matrix = &identityMatrix;
	camera->SetWorldMatrix(1, &matrix);

	// render the geometry
	camera->Render(desc);
}

//
// DRAW A 2D LINE
//
void GraphicLine (ZeroCamera *camera,
				  const ZeroColor color,
				  const ZeroPoint<float> &p0,
				  const ZeroPoint<float> &p1,
				  const float z)
{
	// get the camera viewport
	const ZeroViewport *viewport = &camera->GetViewport();
	const float l = (float)(viewport->pane.x0);
	const float r = (float)(viewport->pane.x1-1);
	const float t = (float)(viewport->pane.y0);
	const float b = (float)(viewport->pane.y1-1);

	// if the line is outside the view...
	if ((p0.x >= r) ||
		(p0.y >= b) ||
		(p1.x <= l) ||
		(p1.y <= t))
	{
		// trivial rejection
		return;
	}

	// project z
	const float rhw = 1.0f / z;
	const float vz = viewport->homScalez0 + viewport->homScalez1 * rhw;

	// create the render descriptor
	ZeroRenderDescriptor desc(camera);
	desc.primitive = ZERO_PRIMITIVE_LINESTRIP;
	desc.format = ZERO_FORMAT_RHW | ZERO_FORMAT_DIFFUSE;
	if ((p0.x <= l) ||
		(p0.y <= t) ||
		(p1.x >= r) ||
		(p1.y >= b))
	{
		desc.flags = ZERO_RENDERFLAG_CLIP;
	}
	desc.vertexCount = 2;
	desc.indexCount = 0;

	// lock the render descriptor
	camera->Lock(desc, true);

	// start
	desc.vertexRecord.AddVertexPos(p0.x, p0.y, false, vz, rhw);
	desc.vertexRecord.AddVertexColor(color);

	// end
	desc.vertexRecord.AddVertexPos(p1.x, p1.y, true, vz, rhw);
	desc.vertexRecord.AddVertexColor(color);

	// unlock the render descriptor
	camera->Unlock(desc);

	// rewind pointers
	desc.vertexRecord.RewindPointersBy(desc.vertexCount);

	// set the render state
	camera->SetRenderStates(renderStates);
	camera->SetTextureStages(0, NULL, NULL);

	// set the world matrix
	const ZeroMatrix *matrix = &identityMatrix;
	camera->SetWorldMatrix(1, &matrix);

	// render the geometry
	camera->Render(desc);
}

//
// DRAW A 2D FILLED RECTANGLE
//
void GraphicRectFilled (ZeroCamera *camera,
						const ZeroColor color,
						const ZeroArea<float> &rect,
						const float z)
{
	// get the camera viewport
	const ZeroViewport *viewport = &camera->GetViewport();
	const float l = (float)(viewport->pane.x0);
	const float r = (float)(viewport->pane.x1-1);
	const float t = (float)(viewport->pane.y0);
	const float b = (float)(viewport->pane.y1-1);

	// if the rectangle is outside the view...
	if ((rect.p0.x >= r) ||
		(rect.p0.y >= b) ||
		(rect.p1.x <= l) ||
		(rect.p1.y <= t))
	{
		// trivial rejection
		return;
	}

	// project z
	const float rhw = 1.0f / z;
	const float vz = viewport->homScalez0 + viewport->homScalez1 * rhw;

	// create the render descriptor
	ZeroRenderDescriptor desc(camera);
	desc.primitive = ZERO_PRIMITIVE_TRIANGLEFAN;
	desc.format = ZERO_FORMAT_RHW | ZERO_FORMAT_DIFFUSE;
	if ((rect.p0.x <= l) ||
		(rect.p0.y <= t) ||
		(rect.p1.x >= r) ||
		(rect.p1.y >= b))
	{
		desc.flags = ZERO_RENDERFLAG_CLIP;
	}
	desc.vertexCount = 4;
	desc.indexCount = 0;

	// lock the render descriptor
	camera->Lock(desc, true);

	// upper left
	desc.vertexRecord.AddVertexPos(rect.p0.x, rect.p0.y, false, vz, rhw);
	desc.vertexRecord.AddVertexColor(color);

	// lower left
	desc.vertexRecord.AddVertexPos(rect.p0.x, rect.p1.y, true, vz, rhw);
	desc.vertexRecord.AddVertexColor(color);

	// upper right
	desc.vertexRecord.AddVertexPos(rect.p1.x, rect.p0.y, true, vz, rhw);
	desc.vertexRecord.AddVertexColor(color);

	// lower right
	desc.vertexRecord.AddVertexPos(rect.p1.x, rect.p1.y, true, vz, rhw);
	desc.vertexRecord.AddVertexColor(color);

	// unlock the render descriptor
	camera->Unlock(desc);

	// rewind pointers
	desc.vertexRecord.RewindPointersBy(desc.vertexCount);

	// set the render state
	camera->SetRenderStates(renderStates);
	camera->SetTextureStages(0, NULL, NULL);

	// set the world matrix
	const ZeroMatrix *matrix = &identityMatrix;
	camera->SetWorldMatrix(1, &matrix);

	// render the geometry
	camera->Render(desc);
}

//
// DRAW A 2D EMPTY RECTANGLE
//
void GraphicRectEmpty(ZeroCamera *camera,
					  const ZeroColor color,
					  const ZeroArea<float> &rect,
					  const float z)
{
	// get the camera viewport
	const ZeroViewport *viewport = &camera->GetViewport();
	const float l = (float)(viewport->pane.x0);
	const float r = (float)(viewport->pane.x1-1);
	const float t = (float)(viewport->pane.y0);
	const float b = (float)(viewport->pane.y1-1);

	// if the rectangle is outside the view...
	if ((rect.p0.x >= r) ||
		(rect.p0.y >= b) ||
		(rect.p1.x <= l) ||
		(rect.p1.y <= t))
	{
		// trivial rejection
		return;
	}

	// project z
	const float rhw = 1.0f / z;
	const float vz = viewport->homScalez0 + viewport->homScalez1 * rhw;

	// create the render descriptor
	ZeroRenderDescriptor desc(camera);
	desc.primitive = ZERO_PRIMITIVE_LINESTRIP;
	desc.format = ZERO_FORMAT_RHW | ZERO_FORMAT_DIFFUSE;
	if ((rect.p0.x <= l) ||
		(rect.p0.y <= t) ||
		(rect.p1.x >= r) ||
		(rect.p1.y >= b))
	{
		desc.flags = ZERO_RENDERFLAG_CLIP;
	}
	desc.vertexCount = 5;
	desc.indexCount = 0;

	// lock the render descriptor
	camera->Lock(desc, true);

	// upper left
	desc.vertexRecord.AddVertexPos(rect.p0.x, rect.p0.y, false, vz, rhw);
	desc.vertexRecord.AddVertexColor(color);

	// upper right
	desc.vertexRecord.AddVertexPos(rect.p1.x, rect.p0.y, true, vz, rhw);
	desc.vertexRecord.AddVertexColor(color);

	// lower right
	desc.vertexRecord.AddVertexPos(rect.p1.x, rect.p1.y, true, vz, rhw);
	desc.vertexRecord.AddVertexColor(color);

	// lower left
	desc.vertexRecord.AddVertexPos(rect.p0.x, rect.p1.y, true, vz, rhw);
	desc.vertexRecord.AddVertexColor(color);

	// upper left
	desc.vertexRecord.AddVertexPos(rect.p0.x, rect.p0.y, true, vz, rhw);
	desc.vertexRecord.AddVertexColor(color);

	// unlock the render descriptor
	camera->Unlock(desc);

	// rewind pointers
	desc.vertexRecord.RewindPointersBy(desc.vertexCount);

	// set the render state
	camera->SetRenderStates(renderStates);
	camera->SetTextureStages(0, NULL, NULL);

	// set the world matrix
	const ZeroMatrix *matrix = &identityMatrix;
	camera->SetWorldMatrix(1, &matrix);

	// render the geometry
	camera->Render(desc);
}
