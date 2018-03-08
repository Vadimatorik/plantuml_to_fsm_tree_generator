Plantuml to fsm tree generator
=====================

Данный репозитолрий содержит исходный код программы, генерирующий описание бинарного дерева для [module_fsm](https://github.com/Vadimatorik/module_fsm) из PlanUML файла.

Пример работы
---------------------
Входной файл test.pu

```plantuml
@startuml

state "spi_failure_handler" as fspi {
fspi: SPI перестал работать.
fspi: Обработка исключительной 
fspi: ситуации.
fspi: С последующим выходом.
}

[*] --> s1 

state "gui_init" as s1 {
s1: Инициализация графической 
s1: оболочки.
}

s1 --> s2     : 0
s1 --> fspi   : 1

state "dp_init" as s2 {
s2: Инициализация цифровых
s2: потенциометров.
}

s2 --> fspi   : 2
s2 --> s3     : 0

state "further_actions" as s3 {
s3: Какие-то дальнейшие действия
s3: После чего выход.
}

@enduml
```
Вид диаграммы состояний в PlantUML:

![Вид диаграммы состояний в PlantUML](https://image.ibb.co/j3Esf7/board.png)
