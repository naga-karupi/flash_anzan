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
	Font description_font{ FontMethod::MSDF, 40, Typeface::Bold };

public:
	GameWaitingScene(const InitData& init) : IScene(init){
		//pass
	}

	~GameWaitingScene() {
		//pass
	}

	void draw()const override {
		Scene::SetBackground(ColorF{ 0.0, 0.0, 0.2 });
		description_font(U"　　space キーを押すと\nカウントダウンを開始します。").draw(Arg::center = Vec2{ 400, 300 }, ColorF{1.0, 1.0, 1.0});
	}

	void update() override {
		if (KeySpace.down()) {
			changeScene(U"CountDownScene", 0ms);
		}
	}
};

class CountDownScene : public App::Scene {
	Stopwatch stopwatch;
	Font sec_font{ FontMethod::MSDF, 40, Typeface::Bold };

public:
	CountDownScene(const InitData& init) : IScene(init), stopwatch(StartImmediately::Yes) {
		//pass
	}

	~CountDownScene() {
		//pass
	}

	void draw() const override {
		const Seconds sec = 3s - Seconds(stopwatch.s());
		if(sec != 0s)
			sec_font(U"{}"_fmt(sec.count())).drawAt(50, Vec2{ 400, 300 }, ColorF{ 1.0, 1.0, 1.0 });
		else
			sec_font(U"Start!").drawAt(50, Vec2{ 400, 300 }, ColorF{ 1.0, 1.0, 1.0 });
	}

	void update() override {
		if (stopwatch.s() >= 4) {
			changeScene(U"PlayGameScene", 0ms);
		}
	}

};



void Main()
{
	Scene::SetBackground(ColorF{ 0.0, 0.0, 0.2 });

	App manager;

	manager.add<Title>(U"Title");
	manager.add<GameWaitingScene>(U"GameWaitingScene");
	manager.add<CountDownScene>(U"CountDownScene");

	const Seconds sec = 2s - 1s;
	
	while (System::Update())
	{
		if (not manager.update()) {
			break;
		}
		
	}
}
