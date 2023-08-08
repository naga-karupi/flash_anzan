# include <Siv3D.hpp> // OpenSiv3D v0.6.10

struct GameInfo{
	int digit_num;
	int phase_num;
	SecondsF all_seconds;
};

struct AllInfo {
	GameInfo info;
	int answer;
	int correct_ans;
	String mode;
	std::shared_ptr<JSON> json_ptr;
};

static constexpr auto white_color = ColorF{1.0, 1.0, 1.0};
static constexpr auto background_color = ColorF{ 0.0, 0.0, 0.2 };

using App = SceneManager<String, AllInfo>;

class ESCInterrupter : public App::Scene {
	bool is_escape = false;
	Font pause_font{ FontMethod::MSDF, 48, Typeface::Bold };

public:
	ESCInterrupter(const InitData& init) : IScene(init) {

	}

	Array<Stopwatch> stopwatches;

	void interruptDraw() const {
		if (is_escape) {

		}
	}

	void interruptUpdate() {
		bool is_rise;
		is_escape ^= is_rise = KeyS.down();

		if (is_rise) {
			for (auto& stopwatch : stopwatches) {
				if (not stopwatch.isStarted())
					continue;

				stopwatch.isRunning() ? stopwatch.pause() : stopwatch.resume();
			}
		}


		if (is_escape) {
//			Rect(0, 0, 800, 600).draw(ColorF{ 0.0, 0.0, 0.2, 0.5 });
			pause_font(U"Pause").drawAt(Vec2{ 400, 200 });

			if (SimpleGUI::ButtonAt(U"タイトルに戻る", Vec2{ 400, 400 })) {
				changeScene(U"Title");
			}
		}
	}
};

/// @brief タイトル画面を表示するクラス
class Title : public App::Scene {
	Font title_font{ FontMethod::MSDF, 48, Typeface::Bold };

public:
	Title(const InitData& init) : IScene(init) {
		if (!FileSystem::IsFile(U"history.json")) {
			JSON json;
			json[U"app"] = U"flash_anzan";
			json.save(U"history.json");
			getData().json_ptr = std::make_shared<JSON>(JSON::Load(U"history.json"));
				
		}else
			getData().json_ptr = std::make_shared<JSON>(JSON::Load(U"history.json"));

	}

	~Title() {
		//pass
	}

	void draw()const override {
		Scene::SetBackground(background_color);
		title_font(U"FLASH 暗算").draw(64, Vec2{ 20, 330 }, white_color);
	}

	void update() override {
		if (SimpleGUI::Button(U"easy", Vec2{ 20, 420 }, 120)) {
			getData().info.all_seconds = 10s;
			getData().info.digit_num = 1;
			getData().info.phase_num = 10;
			getData().mode = U"easy";
			changeScene(U"GameWaitingScene");
		}

		if (SimpleGUI::Button(U"hard", Vec2{ 160, 420 }, 120)) {
			getData().info.all_seconds = 7.5s;
			getData().info.digit_num = 3;
			getData().info.phase_num = 10;
			getData().mode = U"hard";
			changeScene(U"GameWaitingScene");
		}

		if (SimpleGUI::Button(U"custom", Vec2{ 300, 420 }, 120)) {
			changeScene(U"Custom", 0s);
		}

		if (SimpleGUI::Button(U"Log", Vec2{ 700, 50 })) {
			changeScene(U"LogScene");
		}
	}
};

/// @brief 任意の数値を設定するための画面
class Custom: public App::Scene {
public:
	TextEditState all_seconds;
	TextEditState digit_num;
	TextEditState phase_num;

	Font description{ FontMethod::MSDF, 48 };

	Custom(const InitData& init) : IScene(init) {
		Scene::SetBackground(background_color);

		all_seconds.text = U"10";
		digit_num.text = U"1";
		phase_num.text = U"10";
	}

	~Custom() {
		//pass
	}

	void draw()const override {
		description(U"全表示時間：").draw(20, Vec2{ 215, 220 }, white_color);
		description(U"桁数　　　：").draw(20, Vec2{ 215, 270 }, white_color);
		description(U"表示数　　：").draw(20, Vec2{ 215, 320 }, white_color);
		
	}

	void update() override {
		SimpleGUI::TextBox(all_seconds, Vec2{ 365, 220 });
		SimpleGUI::TextBox(digit_num, Vec2{ 365, 270 });
		SimpleGUI::TextBox(phase_num, Vec2{ 365, 320 });

		if (SimpleGUI::Button(U"Back", Vec2{ 20, 20 })) {
			changeScene(U"Title");
		}

		if (SimpleGUI::Button(U"Start", Vec2{ 360, 400 })) {
			if (!all_seconds.text.isEmpty()
				&& !digit_num.text.isEmpty()
				&& !phase_num.text.isEmpty()) {

				double all_seconds_d = Parse<double>(all_seconds.text);
				int digit_num_i = Parse<int>(digit_num.text);
				int phase_num_i = Parse<int>(phase_num.text);

				if (all_seconds_d < 0.0
					|| digit_num_i < 0
					|| phase_num_i < 0)
					return;

				getData().info.all_seconds = SecondsF(all_seconds_d);
				getData().info.digit_num = digit_num_i;
				getData().info.phase_num = phase_num_i;
				getData().mode = U"custom";

				changeScene(U"GameWaitingScene", 0s);
			}
		}

	}

};

/// @brief ゲーム開始前の一時待機画面
class GameWaitingScene : public ESCInterrupter {
	Font description_font{ FontMethod::MSDF, 40, Typeface::Bold };

public:
	GameWaitingScene(const InitData& init) : ESCInterrupter(init){
		Scene::SetBackground(background_color);
	}

	~GameWaitingScene() {
		//pass
	}

	void draw()const override {
		description_font(U"　　space キーを押すと\nカウントダウンを開始します。").draw(Arg::center = Vec2{ 400, 300 }, white_color);

		interruptDraw();
	}

	void update() override {
		if (KeySpace.down()) {
			changeScene(U"CountDownScene", 0ms);
		}

		interruptUpdate();
	}
};

/// @brief カウントダウン・スタート表示
class CountDownScene : public ESCInterrupter {
	Font sec_font{ FontMethod::MSDF, 40, Typeface::Bold };

public:
	CountDownScene(const InitData& init) : ESCInterrupter(init) {
		Scene::SetBackground(background_color);
		stopwatches.push_back(Stopwatch());
		stopwatches[0].start();
	}

	~CountDownScene() {
		//pass
	}

	void draw() const override {
		const Seconds sec = 3s - Seconds(stopwatches[0].s());
		if(sec != 0s)
			sec_font(Format(sec.count())).drawAt(50, Vec2{ 400, 300 }, white_color);
		else
			sec_font(U"Start!").drawAt(50, Vec2{ 400, 300 }, white_color);

		interruptDraw();
	}

	void update() override {
		if (stopwatches[0].s() >= 4) {
			changeScene(U"PlayGameScene", 0ms);
		}

		interruptUpdate();
	}

};

/// @brief 実際のゲームの表示
class PlayGameScene : public ESCInterrupter {

	Array<int> phase_nums;
	int now_phase;

	SecondsF one_pannel_sec;

	TextEditState ans_text;

	Font show_font{ FontMethod::MSDF, 40, Typeface::Bold };


public:
	PlayGameScene(const InitData& init)
		: ESCInterrupter(init), phase_nums(getData().info.phase_num),
		now_phase(0), one_pannel_sec(getData().info.all_seconds / getData().info.phase_num) {

		Scene::SetBackground(background_color);

		set_rand();

		stopwatches.push_back(Stopwatch());
		stopwatches[0].start();
	}

	~PlayGameScene() {
		//pass
	}

	void draw() const override {
		int show_phase = stopwatches[0].sF() / one_pannel_sec.count();

		if (show_phase >= phase_nums.size() || (show_phase + 1) * one_pannel_sec.count() - stopwatches[0].sF() < 0.1) {
			return;
		}

		show_font(Format(phase_nums[show_phase])).drawAt(50, Vec2{ 400, 300 });

		interruptDraw();
	}

	void update() override {
		if (stopwatches[0].sF() >= getData().info.all_seconds.count()) {
			changeScene(U"AnsScene", 0s);
		}

		interruptUpdate();
	}

private:
	void set_rand() {
		int answer = 0;

		int max_num = 1;
		int mini_num = 1;

		for (int i = 0; i < getData().info.digit_num; i++) {
			max_num *= 10;
			mini_num *= 10;
		}

		max_num--;
		mini_num /= 10;

		for (auto& num : phase_nums) {
			answer += num = Random(mini_num, max_num);
		}

		getData().correct_ans = answer;
	}
};

class AnsScene : public App::Scene {
	TextEditState text;

public:
	AnsScene(const InitData& init) :IScene(init) {
		Scene::SetBackground(background_color);
	}

	~AnsScene() {
		//pass
	}

	void draw() const override {

	}

	void update() override {
		SimpleGUI::TextBoxAt(text, Vec2{ 400, 300 });

		if (SimpleGUI::ButtonAt(U"解答する", Vec2{ 400, 350 })
			&& !text.text.isEmpty()) {

			bool is_all_num = true;

			for (auto& t : text.text) {
				if (t < U'0' || t > U'9') {
					is_all_num = false;
					break;
				}
			}

			if (!is_all_num) {
				return;
			}

			getData().answer = Parse<int>(text.text);

			if (getData().answer <= 0) {
				return;
			}

			changeScene(U"ResultScene", 0s);
		}
	}
};

class ResultScene  : public App::Scene{
	bool isCorrect;
	Font font{ FontMethod::MSDF, 40, Typeface::Bold };

public:
	ResultScene(const InitData& init) : IScene(init) {
		Scene::SetBackground(background_color);
		isCorrect = (getData().correct_ans == getData().answer);

		JSON json_tmp;

		json_tmp[U"result"] = (isCorrect ? U"correct" : U"uncorrect");
		json_tmp[U"mode"] = getData().mode;

		if (getData().mode != U"easy" && getData().mode != U"hard") {
			json_tmp[U"seconds"] = getData().info.all_seconds.count();
			json_tmp[U"digit"] = getData().info.digit_num;
			json_tmp[U"phase"] = getData().info.phase_num;	
		}
		
		

		(*(getData().json_ptr))[U"log"].push_back(json_tmp);

		(*getData().json_ptr).save(U"history.json");
	}
	~ResultScene() {
		//pass
	}

	void draw() const override {
		if (isCorrect)
			Circle{ 400,300, 110 }.drawFrame(10, 10, Palette::Red);
		else {
			Shape2D::Cross(160, 30, Vec2{ 400, 300 }).draw(Palette::Blue);
			font(U"正解は ", getData().correct_ans).drawAt(50, Vec2{ 400,360 });
		}
		font(isCorrect ? U"正解！" : U"不正解・・・").drawAt(50, Vec2{ 400, 300 });
	}

	void update() override {
		if (SimpleGUI::ButtonAt(U"完了", Vec2{ 400, 500 })) {
			changeScene(U"Title");
		}
	}
};

class LogScene : public App::Scene {
	JSON json;
	Font log_font{ FontMethod::MSDF, 40 };
public:
	LogScene(const InitData& init): IScene(init) {
		json = JSON::Load(U"history.json");
	}

	~LogScene() {
		//pass
	}

	void draw() const override {
		Rect{ 100, 150, 600, 350 }.draw(white_color);
		auto size = json[U"log"].size();
		auto& log = json[U"log"];

		constexpr auto black = ColorF{ 0.0, 0.0, 0.0 };

		log_font(U"直近三回の結果を表示します").drawAt(30, Vec2{ 400,75 });

		const int loop_max = log.size() >= 3 ? 3 : log.size();

		for (int i = 1; i <= loop_max; i++) {
			log_font(Format(i)).draw(30, Vec2{ 150, 100 + 100 * i }, black);
			log_font(log[size - i][U"mode"].getString()).draw(30, Vec2{ 300, 100 + 100 * i }, black);
			log_font(log[size - i][U"result"].getString()).draw(30, Vec2{ 500, 100 + 100 * i }, black);
		}

	}

	void update() override {
		if (SimpleGUI::Button(U"戻る", Vec2{ 30, 30 })) {
			changeScene(U"Title");
		}
	}
};

void Main() {
	Window::SetTitle(U"flash anzan");
	Scene::SetBackground(background_color);

	App manager;

	manager.add<Title>(U"Title");
	manager.add<Custom>(U"Custom");
	manager.add<GameWaitingScene>(U"GameWaitingScene");
	manager.add<CountDownScene>(U"CountDownScene");
	manager.add<PlayGameScene>(U"PlayGameScene");
	manager.add<AnsScene>(U"AnsScene");
	manager.add<ResultScene>(U"ResultScene");
	manager.add<LogScene>(U"LogScene");
	
	while (System::Update()) {
		if (not manager.update()) {
			break;
		}
		
	}

	
}
