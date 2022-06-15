#include "Scene.h"
#include "math/math.h"

Intersection SceneIntersector::Intersect(const Ray& r, uint32 primitiveIndex) {
	uint32 indexOffset = primitiveIndex * 3;
	uint32 v0i = (*indexPool)[indexOffset], v1i = (*indexPool)[indexOffset + 1],
		v2i = (*indexPool)[indexOffset + 2];
	Intersection inter;
	Vertex v0 = (*vertexPool)[v0i], v1 = (*vertexPool)[v1i], v2 = (*vertexPool)[v2i];
	if (Math::ray_intersect(v0.position,v1.position,v2.position,r,&inter.t,
		&inter.localUv,&inter.position)) {
		//recompute the uv
		
		Vector2f e1 = Math::vsub(v1.uv, v0.uv);
		Vector2f e2 = Math::vsub(v2.uv, v0.uv);
		inter.uv = Math::vadd(
			Math::vadd(
				Math::vmul(e1, inter.localUv.x),
				Math::vmul(e2, inter.localUv.y)
			),
			v0.uv
		);

		inter.intersected = true;
	}
	else {
		inter.intersected = false;
	}

	return inter;
}


void Scene::Build() {
	al_profile_event();

	//TODO currently we assume the scene objects have different model and textures
	al_for(i,0,sceneObjects.size()) {
		ptr<Model> model = sceneObjects[i]->GetModel();
		const Mat4x4& world = sceneObjects[i]->GetTransform().GetMatrix();
		const Mat4x4& transInvWorld = sceneObjects[i]->GetTransform().GetTransInvMatrix();

		uint32 materialOffset = materialPool.size();
		uint32 textureOffset = texturePool.size();
		al_for(j,0,model->TextureCount()) {
			texturePool.push_back(model->GetTexture(j));
		}
		al_for(j, 0, model->MaterialCount()) {
			Material mat = model->GetMaterial(j);
			al_for(k ,0 ,TEXTURE_TYPE_NUM){
				if (mat.textureIndex[i] >= 0) {
					mat.textureIndex[i] += textureOffset;
				}
			}
		}
		al_for(j,0,model->MeshCount()) {
			ptr<Mesh> mesh = model->GetMesh(j);
			uint32 vertexOffset = vertexPool.size(),indiceOffset = indexPool.size();
			const vector<Vertex>& vertices = mesh->GetVertices();
			vertexPool.insert(vertexPool.end(), vertices.begin(), vertices.end());
			al_for(k,0,vertices.size()) {
				//adjust the vertex's position by transform
				Vertex& vert = vertexPool[k + vertexOffset];
				vert.normal = Math::transform_vector(transInvWorld,vert.normal);
				vert.tangent = Math::transform_vector(transInvWorld, vert.tangent);
				vert.position = Math::transform_point(world, vert.position);
			}
			const vector<uint32>& indice = mesh->GetIndices();
			indexPool.insert(indexPool.end(), indice.begin(), indice.end());
			al_for(k,0,indice.size()) {
				indexPool[indiceOffset + k] += vertexOffset;
			}
			uint32 matIndex = model->GetMeshMaterialIndex(j);
			uint32 primitiveCount = indice.size() / 3;
			primitiveMaterialIndex.insert(primitiveMaterialIndex.end(), primitiveCount, matIndex + materialOffset);
		}
	}

	al_assert(indexPool.size() % 3 == 0, "Scene::Build we only support triangle mesh so the index pool must be multiple of 3");
	uint32 primitiveCount = indexPool.size() / 3;
	vector<BVHPrimitive> primitives;
	primitives.reserve(primitiveCount);
	
	al_for(i, 0, primitiveCount) {
		uint32 v0 = indexPool[i * 3], v1 = indexPool[i * 3 + 1],
			v2 = indexPool[i * 3 + 2];
		Bound3f bound;
		bound = Math::bound_merge(bound, vertexPool[v0].position);
		bound = Math::bound_merge(bound, vertexPool[v1].position);
		bound = Math::bound_merge(bound, vertexPool[v2].position);

		primitives.push_back(BVHPrimitive(bound));
	}
	SceneIntersector* intersector = al_new(SceneIntersector,&indexPool,&vertexPool);

	tree.Build(primitives, intersector);
}

ptr<Model>   Scene::GetModel(ModelID i) {
	al_assert(i < models.size() && i >= 0, "Scene::GetModel index {0} is out of bondary {1}",i,models.size());
	return models[i];
}

ptr<SceneObject> Scene::GetSceneObject(SceneObjectID i) {
	al_assert(i < sceneObjects.size() && i >= 0, "Scene::GetSceneObject index {0} is out of bondary {1}", i, models.size());
	return sceneObjects[i];
}

ModelID Scene::LoadModel(const String& path) {
	if (auto iter = std::find(modelPaths.begin(), modelPaths.end(), path);iter != modelPaths.end()) {
		al_log("model {0} is found at {1}", WideString2String(path), iter - modelPaths.begin());
		return iter - modelPaths.begin();
	}
	ptr<Model> model = Model::Load(path);
	modelPaths.push_back(path);
	if (model != nullptr) {
		models.push_back(model);
	}
}

SceneObjectID Scene::CreateSceneObject(ptr<Model> model, const Transform& transform) {
	ptr<SceneObject> sobj(al_new(SceneObject, model, transform));
	sceneObjects.push_back(sobj);
	return (int32)sceneObjects.size() - 1;
}


ptr<Texture>  Scene::GetTexture(uint32 texId){
	al_assert(texId < texturePool.size(), "Scene::GetTexture texture is out of bondary");
	return texturePool[texId];
}

SceneObject::SceneObject(ptr<Model>& model, const Transform& transform):model(model),
	transform(transform){
	al_assert(model != nullptr, "SceneObject::SceneObject : model of a scene object should not be nullptr");
}


IntersectSurfaceInfo Scene::Intersect(const Ray& r) {
	BVHIntersectInfo bvhInfo = tree.Intersect(r);
	
	IntersectSurfaceInfo info;
	info.intersection = bvhInfo.intersection;

	uint32 indexOffset = bvhInfo.primitiveIndex * 3;
	uint32 v0i = indexPool[indexOffset], v1i = indexPool[indexOffset + 1],
		v2i = indexPool[indexOffset + 2];

	const Vertex& v0 = vertexPool[v0i], & v1 = vertexPool[v1i],
		& v2 = vertexPool[v2i];
		
	info.material = &materialPool[primitiveMaterialIndex[bvhInfo.primitiveIndex]];
	if (info.material->textureIndex[TEXTURE_TYPE_NORMALS] >= 0) {
		ptr<Texture> normalMap = GetTexture(info.material->textureIndex[TEXTURE_TYPE_NORMALS]);
		
		Vector4f localNormalSample = normalMap->Sample(bvhInfo.intersection.uv, TEXTURE_SAMPLER_LINEAR);
		
		Vector3f localNormal = Vector3f(localNormalSample.x, localNormalSample.y, localNormalSample.z);
		localNormal = Math::normalize(Math::vsub(Math::vmul(localNormal, 2.f), Vector3f(1.f, 1.f, 1.f)));

		Vector3f normal = Math::normalize(Math::interpolate3(v0.normal, v1.normal, v2.normal, bvhInfo.intersection.localUv));
		Vector3f tangent = Math::normalize(Math::interpolate3(v0.tangent, v1.tangent, v2.tangent, bvhInfo.intersection.localUv));
		Vector3f bitangent = Math::cross(tangent, normal);

		info.normal = Math::vadd(
			Math::vadd(
				Math::vmul(tangent	, localNormal.x), 
				Math::vmul(bitangent, localNormal.y)
			),
				Math::vmul(normal	, localNormal.z)
		);
		info.normal = Math::normalize(info.normal);
	}
	else {
		info.normal = Math::normalize(Math::interpolate3(v0.normal, v1.normal, v2.normal, bvhInfo.intersection.localUv));
	}


	return info;
}