-- ========================================================
-- 1. СТРУКТУРА БАЗЫ ДАННЫХ (ПОВТОРЕНИЕ ДЛЯ ПОЛНОТЫ ФАЙЛА)
-- ========================================================
DROP VIEW IF EXISTS view_transmitter_specs;
DROP TABLE IF EXISTS specs CASCADE;
DROP TABLE IF EXISTS transmitters CASCADE;
DROP TABLE IF EXISTS objects CASCADE;

CREATE TABLE objects (
    id SERIAL PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    latitude DOUBLE PRECISION DEFAULT 0.0,
    longitude DOUBLE PRECISION DEFAULT 0.0,
);

CREATE TABLE transmitters (
    id SERIAL PRIMARY KEY,
    object_id INTEGER NOT NULL REFERENCES objects(id) ON DELETE CASCADE,
    name VARCHAR(255) NOT NULL,
);

CREATE TABLE specs (
    id SERIAL PRIMARY KEY,
    transmitter_id INTEGER UNIQUE NOT NULL REFERENCES transmitters(id) ON DELETE CASCADE,
    power_watt REAL DEFAULT 0.0,      -- Мощность
    gain_db REAL DEFAULT 0.0,         -- КУ
    antenna_height REAL DEFAULT 0.0   -- Высота подвеса
);

-- ========================================================
-- 2. ГЕНЕРАЦИЯ 10+ ТЕСТОВЫХ ПРИМЕРОВ (ОБЪЕКТЫ -> ПЕРЕДАТЧИКИ)
-- ========================================================

-- ОБЪЕКТ 1: Телецентр "Останкино"
INSERT INTO objects (name, latitude, longitude) VALUES ('Телецентр Останкино', 55.819, 37.611);

INSERT INTO transmitters (object_id, name) VALUES (1, 'Первый Канал (Цифра)');     -- ID 1
INSERT INTO transmitters (object_id, name) VALUES (1, 'Радио Маяк');               -- ID 2
INSERT INTO transmitters (object_id, name) VALUES (1, 'Вести FM');                 -- ID 3

-- ОБЪЕКТ 2: РТС "Северная"
INSERT INTO objects (name, latitude, longitude) VALUES ('РТС Северная', 55.901, 37.550);

INSERT INTO transmitters (object_id, name) VALUES (2, 'Европа Плюс');              -- ID 4
INSERT INTO transmitters (object_id, name) VALUES (2, 'Дорожное Радио');           -- ID 5
INSERT INTO transmitters (object_id, name) VALUES (2, 'Служебный канал МЧС');      -- ID 6
INSERT INTO transmitters (object_id, name) VALUES (2, 'Ретро FM');                 -- ID 7

-- ОБЪЕКТ 3: Мобильная вышка "Юг"
INSERT INTO objects (name, latitude, longitude) VALUES ('Вышка Сотовой Связи №42', 55.600, 37.700);

INSERT INTO transmitters (object_id, name) VALUES (3, 'Базовая станция LTE-2600'); -- ID 8
INSERT INTO transmitters (object_id, name) VALUES (3, 'Сектор A (GSM-900)');       -- ID 9
INSERT INTO transmitters (object_id, name) VALUES (3, 'Сектор B (5G Test)');       -- ID 10

-- ========================================================
-- 3. ЗАПОЛНЕНИЕ ТТХ ДЛЯ ВСЕХ 10 ПЕРЕДАТЧИКОВ
-- ========================================================
-- ТТХ для передатчиков Останкино
INSERT INTO specs (transmitter_id, power_watt, gain_db, antenna_height) VALUES (1, 5000, 18.5, 525);
INSERT INTO specs (transmitter_id, power_watt, gain_db, antenna_height) VALUES (2, 2500, 12.0, 310);
INSERT INTO specs (transmitter_id, power_watt, gain_db, antenna_height) VALUES (3, 1000, 10.5, 280);

-- ТТХ для передатчиков РТС Северная
INSERT INTO specs (transmitter_id, power_watt, gain_db, antenna_height) VALUES (4, 1200, 8.2, 85);
INSERT INTO specs (transmitter_id, power_watt, gain_db, antenna_height) VALUES (5, 800, 7.5, 82);
INSERT INTO specs (transmitter_id, power_watt, gain_db, antenna_height) VALUES (6, 150, 4.0, 95);
INSERT INTO specs (transmitter_id, power_watt, gain_db, antenna_height) VALUES (7, 1100, 8.0, 80);

-- ТТХ для мобильной вышки
INSERT INTO specs (transmitter_id, power_watt, gain_db, antenna_height) VALUES (8, 40, 15.0, 35);
INSERT INTO specs (transmitter_id, power_watt, gain_db, antenna_height) VALUES (9, 20, 12.0, 32);
INSERT INTO specs (transmitter_id, power_watt, gain_db, antenna_height) VALUES (10, 60, 17.5, 38);

-- ========================================================
-- 4. ПРЕДСТАВЛЕНИЕ ДЛЯ ВЫВОДА (VIEW)
-- ========================================================
CREATE OR REPLACE VIEW view_transmitter_specs AS
SELECT 
    transmitter_id,
    'Мощность (Вт)' AS parameter_name, 
    power_watt::text AS parameter_value
FROM specs
UNION ALL
SELECT 
    transmitter_id,
    'КУ (дБ)' AS parameter_name, 
    gain_db::text AS parameter_value
FROM specs
UNION ALL
SELECT 
    transmitter_id,
    'Высота подвеса (м)' AS parameter_name, 
    antenna_height::text AS parameter_value
FROM specs;

-- ========================================================
-- 5. ТЕСТОВЫЙ ЗАПРОС (ПРОВЕРКА)
-- ========================================================
-- Посмотреть все данные иерархией:
SELECT 
    obj.name AS "Объект", 
    tx.name AS "Передатчик", 
    s.power_watt AS "Мощность", 
    s.gain_db AS "КУ", 
    s.antenna_height AS "Высота"
FROM objects obj
JOIN transmitters tx ON obj.id = tx.object_id
JOIN specs s ON tx.id = s.transmitter_id
ORDER BY obj.name;
