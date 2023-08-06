# include <Siv3D.hpp> // OpenSiv3D v0.6.10

struct GameInfo{
	int digit_num;
	int phase_num;
	SecondsF all_seconds;
};

using App = SceneManager<String, GameInfo>;

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
		if (SimpleGUI::Button(U"easy", Vec2{ 20, 420 }, 120)) {
			getData().all_seconds = 10s;
			getData().digit_num = 1;
			getData().phase_num = 10;
			changeScene(U"GameWaitingScene");
		}

		if (SimpleGUI::Button(U"hard", Vec2{ 160, 420 }, 120)) {
			getData().all_seconds = 10s;
			getData().digit_num = 1;
			getData().phase_num = 10;
			changeScene(U"GameWaitingScene");
		}

		if (SimpleGUI::Button(U"custom", Vec2{ 300, 420 }, 120)) {
			changeScene(U"Custom", 0s);
		}
	}
};

class Custom: public App::Scene {
public:
	TextEditState all_seconds;
	TextEditState digit_num;
	TextEditState phase_num;

	Custom(const InitData& init) : IScene(init) {
		all_seconds.text = U"10";
		digit_num.text = U"1";
		phase_num.text = U"10";
	}

	~Custom() {
		//pass
	}

	void draw()const override {
		
	}

	void update() override {
		SimpleGUI::TextBox(all_seconds, Vec2{ 100,100 });
		SimpleGUI::TextBox(digit_num, Vec2{ 100,150 });
		SimpleGUI::TextBox(phase_num, Vec2{ 100,200 });

		if (SimpleGUI::Button(U"Start", Vec2{ 300, 200 })) {
			if (!all_seconds.text.isEmpty()
				&& !digit_num.text.isEmpty()
				&& !phase_num.text.isEmpty()) {

				getData().all_seconds = SecondsF(Parse<double>(all_seconds.text));
				getData().digit_num = Parse<int>(digit_num.text);
				getData().phase_num = Parse<int>(phase_num.text);

				changeScene(U"GameWaitingScene", 0s);
			}
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

class PlayGameScene : public App::Scene {

public:

};



void Main()
{
	Scene::SetBackground(ColorF{ 0.0, 0.0, 0.2 });

	App manager;

	manager.add<Title>(U"Title");
	manager.add<Custom>(U"Custom");
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
