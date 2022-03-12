# include <Siv3D.hpp> // OpenSiv3D v0.6.3

// 定数
constexpr Size sceneSize = DisplayResolution::FHD_1920x1080;
constexpr int ButtonHeight = 40;
constexpr double UPDATE_FRAME = 0.2; // 単位 : 秒


namespace cgol {
	using cell_t = int;
	enum class state_t : cell_t {
		LIVE = 0,
		DEAD = 1,
		LIVE_ACTIVE = 2,
		DEAD_ACTIVE = 3,
	};

	state_t inv(state_t s) {
		if (state_t::LIVE == s)
			return state_t::DEAD;
		else if (state_t::DEAD == s)
			return state_t::LIVE;
	}

	// 維持 : born >= 生存数 > sparse 
	enum rule_t {
		BORN = 3,
		SPARSE = 1,
		DENCE = 4,
	};

	Color getStateColor(state_t s) {
		Color c{};
		switch (s)
		{
		case state_t::LIVE:
			c = Color(200, 200, 200);
			break;
		case state_t::DEAD:
			c = Color(50, 50, 50);
			break;
		case state_t::LIVE_ACTIVE:
			c = Palette::White;
			break;
		case state_t::DEAD_ACTIVE:
			c = Palette::Black;
			break;
		default:
			break;
		}
		return c;
	}

	class Field {

	public:
		Field(int width, int height) : width(width), height(height), simActive(false)
		{
			field = std::vector<state_t>(width * height);
			for (auto& cell : field) {
				cell = state_t::DEAD;
			}
		}

		void update()
		{
			if (!simActive)
				return;

			updateCount += Scene::DeltaTime();
			if (updateCount <= UPDATE_FRAME)
				return;
			updateCount = 0;
			std::vector<state_t> v(width * height);
			for (auto& c : v)
				c = state_t::DEAD;
			for (int x = 0; x < width; x++) {
				for (int y = 0; y < height; y++) {
					const int index = getIndex(x, y);
					v[index] = checkCell(x, y);
				}
			}
			field = v;
		}

		void step1() {
			std::vector<state_t> v(width * height);
			for (auto& c : v)
				c = state_t::DEAD;
			for (int x = 0; x < width; x++) {
				for (int y = 0; y < height; y++) {
					const int index = getIndex(x, y);
					v[index] = checkCell(x, y);
				}
			}
			field = v;
		}

		void showField() {
			const int w = sceneSize.x / width;
			for (auto x : step(width))
			{
				for (auto y : step(height))
				{
					Rect r(w * x, ButtonHeight + w * y, w, w);
					const int index = getIndex(x, y);
					const state_t cellState = getCellState(index);
					if (r.mouseOver())
					{
						//r.draw(getActiveColor(cellState));
					}
					else
					{
						r.draw(getStateColor(cellState));
					}

					if (r.leftClicked() && !simActive) {
						const int index = getIndex(x, y);
						if (field[index] == state_t::LIVE)
							field[index] = state_t::DEAD;
						else
							field[index] = state_t::LIVE;
					}
				}
			}
			// Rect{ 0, 40, sceneSize.x, sceneSize.y}.draw();
		}


		void randamGenerate() {
			if (simActive)
				return;
			std::random_device rd;
			std::mt19937 engine;
			std::uniform_int_distribution<int> dist(0, 1);
			engine.seed(rd());

			for (auto& cell : field) {
				cell = static_cast<state_t>(dist(engine));
			}
		}


		void activeSim() {
			simActive = true;
		}

		void stopSim() {
			simActive = false;
		}

		void printDebug() {
			Print << updateCount;
			Print << U"Atctive : " << simActive;
		}

	private:

		int getIndex(int x, int y) {
			const int index = x + y * width;
			//if (isField(index))
			//	System::Exit();
			return index;
		}

		int isField(int index) {
			return 0 <= index && index < width * height;
		}

		state_t getCellState(int index) {
			return field[index];
		}

		Color getActiveColor(state_t s) {
			if (s == state_t::LIVE)
				return getStateColor(state_t::LIVE_ACTIVE);
			else if (s == state_t::DEAD)
				return getStateColor(state_t::DEAD_ACTIVE);
		}

		// 周辺を調べて生死を判別
		state_t checkCell(int x, int y) {
			int sum = 0;
			const int index = getIndex(x, y);
			for (int i = -1; i <= 1; i++) {
				for (int j = -1; j <= 1; j++) {
					const int target = getIndex(x + i, y + j);
					if (!isField(target) || (i == 0 && j == 0))
						continue;

					if (getCell(target) == state_t::LIVE)
						sum++;
				}
			}
			const auto s = getCell(index);
			return isLive(s, sum);
		}

		state_t isLive(state_t now, int count) {
			if (now == state_t::LIVE) {
				if (BORN >= count && count > SPARSE)
					return state_t::LIVE;
				else if (count > BORN || SPARSE >= count)
					return state_t::DEAD;
			} else if (now == state_t::DEAD) {
				if (BORN == count)
					return state_t::LIVE;
				else
					return state_t::DEAD;
			}
		}

		void setCell(int index, state_t s) {
			if (!isField(index)) {
				Print << U"illegal access : " << index;
				System::Update();
				while (1) {
					;
				}
			}

			field[index] = s;
		}

		const state_t getCell(int index) {
			if (!isField(index)) {
				Print << U"illegal access : " << index;
				System::Update();
				while (1) {
					;
				}
			}
			return field[index];
		}

		std::vector<state_t> field;
		int width;
		int height;
		bool simActive;
		double updateCount;
	};

}


void Main()
{

	Scene::Resize(sceneSize);
	Window::Resize(sceneSize);
	//Window::SetFullscreen(true);
	// 背景の色を設定 | Set background color
	//Scene::SetBackground(ColorF{ 0.8, 0.9, 1.0 });
	TextEditState text;
	cgol::Field f(400, 200);
	//f.randamGenerate();

	while (System::Update())
	{
		//ClearPrint();
		//f.printDebug();

		if (SimpleGUI::Button(U"Start", Vec2{ 0, 0 }))
		{
			f.activeSim();
		}

		if (SimpleGUI::Button(U"End", Vec2{ 100, 0 }))
		{
			f.stopSim();
		}

		if (SimpleGUI::Button(U"Exit", Vec2{ sceneSize.x - 75, 0 }))
		{
			System::Exit();
		}

		if (SimpleGUI::Button(U"1 Step", Vec2{ 200, 0 }))
		{
			Print << U"1 step";
			f.step1();
		}

		if (SimpleGUI::Button(U"random", Vec2{ 300, 0 }))
		{
			f.randamGenerate();
		}

		// 背景表示
		SimpleGUI::TextBox(text, Vec2{ 500, 0 }, 100, 4);
		//Rect{ 0, ButtonHeight, sceneSize.x, sceneSize.y }.draw();

		f.update();
		f.showField();
	}
}

