#include <SFML/Graphics.hpp>
#include<iostream>
#include <ctime>
#include <windows.h>

using namespace sf;

const int M = 20; // высота игрового поля
const int N = 10; // ширина игрового поля

int field[M][N] = { 0 }; // игровое поле


int figures[7][4] = // Массив фигурок-тетрамино
{
	1,3,5,7, // I
	2,4,5,7, // Z
	3,5,4,6, // S
	3,5,4,7, // T
	2,3,5,7, // L
	3,5,7,6, // J
	2,3,4,5, // O
};

struct Point
{
	int x, y;
}a[4], b[4]; // точка на поле + 2 вспомогательных массива

// Проверка на выход за границы игрового поля
bool check()
{
	for (int i = 0; i < 4; i++)
		if (a[i].x < 0 || a[i].x >= N || a[i].y >= M) return 0;
		else if (field[a[i].y][a[i].x]) return 0;

	return 1;

};

bool endGame(RenderWindow *window)
{
	for (int i = 0; i < N; i++)
	{
		if (field[2][i]) 
		{
			window->close();
			return false;
		}
	}
}

void Hide()
{
	HWND Hide;
	AllocConsole();
	Hide = FindWindowA("ConsoleWindowClass", NULL);
	ShowWindow(Hide, 0);
}

int main()
{
	srand(time(0)); 
	Hide();

	// Создание и загрузка текстуры
	Texture texture, texture_background, texture_frame, texture_start, texture_game_over;
	texture.loadFromFile("tiles.png");
	texture_background.loadFromFile("background.png");
	texture_frame.loadFromFile("frame.png");
	texture_start.loadFromFile("start.png");
	texture_game_over.loadFromFile("game_over.png");

	// Создание спрайта
	Sprite sprite(texture), sprite_background(texture_background), sprite_frame(texture_frame), sprite_start(texture_start), sprite_game_over(texture_game_over);

	// Вырезаем из спрайта отдельный квадратик размером 18х18 пикселей
	sprite.setTextureRect(IntRect(0, 0, 18, 18));

	// Переменные для горизонтального перемещения и вращения
	int dx = 0; bool rotate = 0; int colorNum = 1; bool beginGame = true; int n = rand() % 7;

	// Переменные для таймера и задержки
	float timer = 0, delay = 0.4;

	// Часы (таймер)
	Clock clock;

	bool IsRun = true;

	RenderWindow window0(VideoMode(320, 480), "Tetris");

	while (window0.isOpen())
	{
		Event event;
		while (window0.pollEvent(event))
		{
			// Пользователь нажал на «крестик» и хочет закрыть окно?
			if (event.type == Event::Closed)
			{
				window0.close();
				return 0;
			}
		}
		window0.clear(Color::White);
		window0.draw(sprite_start);
		window0.display();
		if (Keyboard::isKeyPressed(Keyboard::Space))
		{
			window0.close();
			break;
		}
	}

	while (true) {
		// Цикл игры. Выполняется, пока открыто окно
		RenderWindow window(VideoMode(320, 480), "Tetris"); // 320, 480

		while (window.isOpen() && IsRun)
		{
			IsRun = endGame(&window);
			if (!IsRun)
			{
				break;
			}

			// Получаем время, прошедшее с начала отсчета, и конвертируем его в секунды
			float time = clock.getElapsedTime().asSeconds();
			clock.restart();
			timer += time;

			// Обрабатываем очередь событий в цикле
			Event event;
			while (window.pollEvent(event))
			{

				// Пользователь нажал на «крестик» и хочет окно закрыть?
				if (event.type == Event::Closed) {
					// тогда закрываем его
					window.close();
					return 0;
				}
				// Была нажата кнопка на клавиатуре?
				if (event.type == Event::KeyPressed)
					// Эта кнопка – стрелка вверх?…
					if (event.key.code == Keyboard::Up) rotate = true;
				// …или же стрелка влево?…
					else if (event.key.code == Keyboard::Left) dx = -1;
				// …ну тогда может это стрелка вправо?
					else if (event.key.code == Keyboard::Right) dx = 1;
			}

			// Нажали кнопку "вниз"? Ускоряем падение тетрамино
			if (Keyboard::isKeyPressed(Keyboard::Down)) delay = 0.05;

			//// Горизонтальное перемещение ////
			for (int i = 0; i < 4; i++) { b[i] = a[i]; a[i].x += dx; }

			// Вышли за пределы поля после перемещения? Тогда возвращаем старые координаты 
			if (!check()) for (int i = 0; i < 4; i++) a[i] = b[i];


			//// Вращение ////
			if (rotate)
			{
				Point p = a[1]; // задаем центр вращения
				for (int i = 0; i < 4; i++)
				{
					int x = a[i].y - p.y; //y-y0
					int y = a[i].x - p.x; //x-x0
					a[i].x = p.x - x;
					a[i].y = p.y + y;
				}
				// Вышли за пределы поля после поворота? Тогда возвращаем старые координаты 
				if (!check()) { for (int i = 0; i < 4; i++) a[i] = b[i]; }

			}

			//// Движение тетрамино вниз (Тик таймера) ////
			if (timer > delay)
			{
				for (int i = 0; i < 4; i++) { b[i] = a[i]; a[i].y += 1; }
				if (!check())
				{
					for (int i = 0; i < 4; i++) field[b[i].y][b[i].x] = colorNum;
					colorNum = 1 + rand() % 7;
					n = rand() % 7;
					for (int i = 0; i < 4; i++)
					{
						a[i].x = figures[n][i] % 2;
						a[i].y = figures[n][i] / 2;
					}

				}
				timer = 0;

			}

			//----ПРОВЕРКА ЛИНИИ----//
			int k = M - 1;
			for (int i = M - 1; i > 0; i--)
			{
				int count = 0;
				for (int j = 0; j < N; j++)
				{
					if (field[i][j]) count++;
					field[k][j] = field[i][j];
				}
				if (count < N) k--;
			}

			// Первое появление тетрамино на поле?
			if (beginGame)
			{
				beginGame = false;
				n = rand() % 7;
				for (int i = 0; i < 4; i++)
				{
					a[i].x = figures[n][i] % 2;
					a[i].y = figures[n][i] / 2;
				}
			}
			dx = 0; rotate = 0; delay = 0.3;

			//----ОТРИСОВКА----//

			// Задаем цвет фона - белый
			window.clear(Color::White);
			window.draw(sprite_background);
			for (int i = 0; i < M; i++)
				for (int j = 0; j < N; j++)
				{
					if (field[i][j] == 0) continue;
					sprite.setTextureRect(IntRect(field[i][j] * 18, 0, 18, 18));
					sprite.setPosition(j * 18, i * 18);
					sprite.move(28, 31); // смещение
					window.draw(sprite);
				}

			for (int i = 0; i < 4; i++)
			{
				// Разукрашиваем тетрамино
				sprite.setTextureRect(IntRect(colorNum * 18, 0, 18, 18));

				// Устанавливаем позицию каждого кусочка тетрамино
				sprite.setPosition(a[i].x * 18, a[i].y * 18);

				sprite.move(28, 31); // смещение

				// Отрисовка спрайта
				window.draw(sprite);
			}
			// Отрисовка фрейма
			window.draw(sprite_frame);

			// Отрисовка окна
			window.display();
		}

		IsRun = true;

		for (int i = 0; i < M; i++)
		{
			for (int j = 0; j < N; j++)
			{
				field[i][j] = 0;
			}
		}

		RenderWindow window2(VideoMode(320, 480), "Tetris");

		while (window2.isOpen())
		{
			Event event;
			while (window2.pollEvent(event))
			{
				// Пользователь нажал на «крестик» и хочет закрыть окно?
				if (event.type == Event::Closed)
				{
					window2.close();
					return 0;
				}
			}
			window2.clear(Color::White);
			window2.draw(sprite_game_over);
			window2.display();
			if (Keyboard::isKeyPressed(Keyboard::Space))
			{
				break;
			}
		}
	}
}
