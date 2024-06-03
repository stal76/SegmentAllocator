# LogAllocator 
Allocator, позволяет выделять и освобождать непрерывные отрезки памяти на массиве индексов.

Подробное описание в 
[segment_allocator.pdf](doc/segment_allocator.pdf)


---

Реализован также простой LogAllocator.


## Порядок работы:

	LogAllocator allocator(0, 100, error_result);
	int size_t index = allocator.Allocate(20);
	allocator.Free(5, 10);

- Создаем объект LogAllocator, параметры: начальный индекс, размер, значение, которое будет возврщаться при неудачной попытке выделить отрезок.
- Пробуем выделить отрезок длины 20.
- Освобождаем ранее выделенный отрезок.

## Как устроен внутри:

Объект LogAllocator хранит все свободные интервалы. При попытке выделить отрезок, выбирается свободный интервал минимальной длины с размером
не меньше требуемой.

Для хранения свободных интервалов используется два set'а. Сложность $O(\log{size})$, здесь $size$ - количество
свободных отрезков. В лучшем случае может быть один большой свободный отрезок.

## Пример работы:

В файле demo_allocator.cpp приведен пример работы.

	LogAllocator allocator(0, 100, 0);
	std::vector<Action> actions;
	allocator.Allocate(20);
	allocator.Allocate(30);
	allocator.Allocate(50);

	// После этого не осталось свободное место, начинаем освобождать
	
	allocator.Free(40, 10);	 // Free when no free segments
	allocator.Free(5, 10);	 // Free when exists only right segment
	allocator.Free(80, 10);	 // Free when exists only left segment
	allocator.Free(52, 8);	 // Free when exists left and right segment

После выполнения получим следующие свободные отрезки: [5,15) [40,50) [52,60) [80,90).

## Ограничения:

- Объект не хранит по отдельности каждый выделенный отрезок, поэтому при освобождении отрезков необходимо указывать кроме позиции также и размер.