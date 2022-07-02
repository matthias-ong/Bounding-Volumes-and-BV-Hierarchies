#include "GameObject.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

void GameObject::SetTransform(Transform trans)
{
	transform = trans;
}
void GameObject::SetModelID(const char* id)
{
	model_id = id;
}
Transform& GameObject::GetTransform()
{
	return transform;
}
const char* GameObject::GetModelID()
{
	return model_id;
}

void GameObject::DrawImGuiControls()
{
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, 85);
	ImGui::Text("Position");
	ImGui::NextColumn();
	ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() / 5.0f);
	ImGui::Button("X");
	ImGui::SameLine();
	ImGui::DragFloat("##PositionX", &transform.Position.x, 0.05f, 0.0f, 0.0f, "%.2f");
	ImGui::SameLine();
	ImGui::Text(" ");
	ImGui::SameLine();
	ImGui::Button("Y");
	ImGui::SameLine();
	ImGui::DragFloat("##PositionY", &transform.Position.y, 0.05f, 0.0f, 0.0f, "%.2f");
	ImGui::SameLine();
	ImGui::Text(" ");
	ImGui::SameLine();
	ImGui::Button("Z");
	ImGui::SameLine();
	ImGui::DragFloat("##PositionZ", &transform.Position.z, 0.05f, 0.0f, 0.0f, "%.2f");
	ImGui::NextColumn();

	//SCALE
	//ImGui::SetColumnWidth(0, 85);
	//ImGui::Text("Scale");
	//ImGui::NextColumn();
	//ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
	//ImGui::SameLine();
	//ImGui::DragFloat("##ScaleX", &transform.scale, 0.05f, 0.0f, 0.0f, "%.2f");
	//ImGui::NextColumn();
	//if (transform.scale < 0)
	//	transform.scale = 0;
	//transform.scale2 = { transform.scale, transform.scale, transform.scale };

	/*
		ImGui::NextColumn();
		ImGui::SetColumnWidth(0, 85);
		ImGui::Text("Rotation");
		ImGui::NextColumn();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() / 5.0f);
		ImGui::Button("X");
		ImGui::SameLine();
		ImGui::DragFloat("##RotationX", &transform.rotation.x, 0.05f, 0.0f, 0.0f, "%.2f");
		ImGui::SameLine();
		ImGui::Text(" ");
		ImGui::SameLine();
		ImGui::Button("Y");
		ImGui::SameLine();
		ImGui::DragFloat("##RotationY", &transform.rotation.y, 0.05f, 0.0f, 0.0f, "%.2f");
		ImGui::SameLine();
		ImGui::Text(" ");
		ImGui::SameLine();
		ImGui::Button("Z");
		ImGui::SameLine();
		ImGui::DragFloat("##RotationZ", &transform.rotation.z, 0.05f, 0.0f, 0.0f, "%.2f");
		ImGui::NextColumn();
		transform.scale = 0.5f;
	*/

	transform.rotation = { 0.f, 0.f, 0.f };
	ImGui::Columns(1);
	const char* oldBV = this->colliderName;
	static const char* items[]{ "AABB", "Ritter's Sphere", "Larsson's EPOS8",  "Larsson's EPOS12",  "Larsson's EPOS24", "PCA Sphere" };
	//ImGui::NextColumn();
	ImGui::ListBox("Bounding Volume", &BV_int, items, IM_ARRAYSIZE(items), 2);
	this->colliderName = items[BV_int];
	if (oldBV != this->colliderName)
		changedCollider = true;

}

void GameObject::DrawImGuiBVH()
{
	//ImGui::Columns(1);
	//const char* oldBV = this->colliderName;
	//static const char* items[]{ "AABB", "Ritter's Sphere", "PCA Sphere" };
	////ImGui::NextColumn();
	//ImGui::ListBox("BVH", &BV_int, items, IM_ARRAYSIZE(items), 2);
	//this->colliderName = items[BV_int];
	//if (oldBV != this->colliderName)
	//	changedCollider = true;
}
