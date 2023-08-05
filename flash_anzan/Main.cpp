# include <Siv3D.hpp> // OpenSiv3D v0.6.10

using App = SceneManager<String>;

class Title : public App::Scene {
	Font title_font{ FontMethod::MSDF, 48, Typeface::Bold };

public:
	Title(const InitData& init) : IScene(init) {
		//pass
	}

	~Title() {
		//pass
	}

	void draw()const override {
		Scene::SetBackground(ColorF{ 0.0, 0.0, 0.2 });
		title_font(U"FLASH 暗算").draw(64, Vec2{ 20, 340 }, ColorF{ 1.0, 1.0, 1.0 });
	}

	void update() override {
		if (SimpleGUI::Button(U"始める", Vec2{ 20, 420 }, 120)) {
			changeScene(U"GameWaitingScene");
		}
	}
};

class GameWaitingScene : public App::Scene {

public:
	GameWaitingScene(const InitData& init) : IScene(init){
		//pass
	}

	~GameWaitingScene() {
		//pass
	}

	void draw()const override {
		Scene::SetBackground(ColorF{ 0.0, 0.0, 0.2 });
	}

	void update() override {
		if (MouseL.down()) {
			changeScene(U"Title");
		}
	}
};

void Main()
{
	Scene::SetBackground(ColorF{ 0.0, 0.0, 0.2 });

	App manager;

	manager.add<Title>(U"Title");
	manager.add<GameWaitingScene>(U"GameWaitingScene");
	
	while (System::Update())
	{
		if (not manager.update()) {
			break;
		}
		
	}
}
