#include "../Objects/Entity.h"
#include <glm/gtc/matrix_transform.hpp>

#include "Utils/Time.h"
#include "Math/Math.h"

#include "Utils/Debug.h"
#include "Window/View.h"
#include "Client/RenderView.h"
#include "General/Settings.h"
#include "Camera.h"

#include "Utils/Data.h"
#include "World/World.h"
#include "Rendering/Objects/GraphicalOverlay.h"

namespace Esteem
{
	cgc::strong_ptr<Camera> Camera::Instantiate(const cgc::strong_ptr<Entity>& entity)
	{
		World* world = entity->GetWorld();

		RenderingFactory* factory = Data::GetRenderingFactory();
		cgc::strong_ptr<RenderCamera> renderCamera = world->CreateRenderCamera();
		cgc::strong_ptr<Camera> camera = world->GetWorldConstituents().cameras.Create(entity, renderCamera);
		if (!world->GetPrimaryCamera())
			world->SetRenderCamera(camera);

		return camera;
	}

// Convenience reasons only
#define entity GetEntity()

	void Camera::Initialize()
	{
		auto& size = View::GetView()->GetScreenSize();

		// weird naming convention
		uintptr_t intPtr = uintptr_t(this);
		while (Rml::GetContext("Default_" + std::to_string(intPtr)))
			++intPtr;

		renderCamera->guiContext = Rml::CreateContext("Default_" + std::to_string(intPtr), { size.x, size.y });

		OnScreenSizeChanged(size);
		View::GetView()->RegisterScreenSizeListener(DELEGATE(&Camera::OnScreenSizeChanged, this));

		entity->SetDirections(glm::mat3(entity->GetRotation()));

		DirtyCleanUp();
	}
	
	Camera::~Camera()
	{
		View::GetView()->UnRegisterScreenSizeListener(DELEGATE(&Camera::OnScreenSizeChanged, this));

		if (!Rml::RemoveContext(renderCamera->guiContext->GetName()))
			Debug::LogWarning("Couldn't remove the HTML context");
	}

	void Camera::Update()
	{
		/*float camSpeed = 20;
		if (Input::GetKeyBoard()->IsKeyDown(Keyboard::LShift))
			camSpeed *= 2;
		else if (Input::GetKeyBoard()->IsKeyDown(Keyboard::LControl))
			camSpeed *= 0.5f;

		//Debug::Log(std::to_string(Time::deltaTime));

		if (Input::GetKeyBoard()->IsKeyDown(Keyboard::W))
			entity->SetPosition(entity->GetPosition() + entity->GetForward() * camSpeed * Time::deltaTime);
		if (Input::GetKeyBoard()->IsKeyDown(Keyboard::S))
			entity->SetPosition(entity->GetPosition() - entity->GetForward() * camSpeed * Time::deltaTime);
		if (Input::GetKeyBoard()->IsKeyDown(Keyboard::A))
			entity->SetPosition(entity->GetPosition() - entity->GetRight() * camSpeed * Time::deltaTime);
		if (Input::GetKeyBoard()->IsKeyDown(Keyboard::D))
			entity->SetPosition(entity->GetPosition() + entity->GetRight() * camSpeed * Time::deltaTime);

		if (Input::GetMouse()->IsButtonDown(Mouse::Button::Right))
		{
			Input::GetMouse()->SetMouseVisible(false);
			Input::GetMouse()->LockCursor(true);

			entity->SetRotation(glm::rotate(entity->GetRotation(), 0.1745329251994329f * Time::deltaTime * Input::GetMouse()->GetPhysicsDeltaMove().y, entity->GetRight()));
			entity->SetRotation(glm::rotate(entity->GetRotation(), 0.1745329251994329f * Time::deltaTime * Input::GetMouse()->GetPhysicsDeltaMove().x, glm::vec3(0, 1, 0)));

			entity->SetForward(	Vector3::forward	* entity->GetRotation());
			entity->SetUp(		Vector3::up			* entity->GetRotation());
			entity->SetRight(	Vector3::right		* entity->GetRotation());
		}
		else
		{
			Input::GetMouse()->SetMouseVisible(true);
			Input::GetMouse()->LockCursor(false);
		}

		entity->SetDirty();*/

		DirtyCleanUp();
	}


	void Camera::OnScreenSizeChanged(const glm::ivec2& screenSize)
	{
		glm::vec2 size = screenSize;
		renderCamera->data.projectionMatrix = glm::perspective(glm::radians(60.0f), size.x / size.y, .1f, 1000.0f);
		renderCamera->data.orthoProjectionMatrix = glm::ortho(0.f, size.x, size.y, 0.f, 0.f, 1000.f);

		if (renderCamera->guiContext)
		{
			renderCamera->guiContext->SetDimensions({ screenSize.x, screenSize.y });
			renderCamera->guiContext->Update();
		}

		// update any graphical overlay that's using this render camera
		auto& overlays = renderCamera->graphicalOverlays;
		for (std::size_t i = 0; i < overlays.size(); ++i)
			overlays[i]->RebuildAllMeshes();
	}

	void Camera::DirtyCleanUp()
	{
		renderCamera->data.position = entity->GetPosition();
		renderCamera->data.forward = entity->GetForward();

		renderCamera->viewMatrix = glm::lookAt(renderCamera->data.position, renderCamera->data.position + renderCamera->data.forward, entity->GetUp());
		renderCamera->data.viewProjectMatrix = renderCamera->data.projectionMatrix * renderCamera->viewMatrix;
		renderCamera->data.invertedViewProjectMatrix = glm::inverse(renderCamera->data.viewProjectMatrix);
	}

	void Camera::AddGraphicOverlay(const cgc::strong_ptr<GraphicalOverlay>& graphicOverlay)
	{
		renderCamera->AddGraphicalOverlay(graphicOverlay);
		graphicOverlay->SetRenderCamera(renderCamera);
	}

	void Camera::RemoveGraphicOverlay(const cgc::strong_ptr<GraphicalOverlay>& graphicOverlay)
	{
		renderCamera->RemoveGraphicalOverlay(graphicOverlay);
		graphicOverlay->SetRenderCamera(cgc::strong_ptr<RenderCamera>());
	}

	Rml::ElementDocument* Camera::LoadHTMLDocument(const std::string& path)
	{
		return renderCamera->LoadHTMLDocument(path);
	}

	void Camera::RemoveHTMLDocument(Rml::ElementDocument* document)
	{
		renderCamera->RemoveHTMLDocument(document);
	}
}

#undef entity