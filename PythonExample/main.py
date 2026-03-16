import sys
from PySide6.QtCore import Qt
from PySide6.QtSql import QSqlDatabase, QSqlQuery
from PySide6.QtWidgets import (QApplication, QMainWindow, QWidget, QHBoxLayout,
                               QVBoxLayout, QTreeWidget, QTreeWidgetItem, QTableWidget,
                               QTableWidgetItem, QPushButton, QHeaderView, QMessageBox,
                               QInputDialog, QStyledItemDelegate, QDoubleSpinBox)


class NumericDelegate(QStyledItemDelegate):
    """Делегат для редактирования числовых значений в таблице"""

    def createEditor(self, parent, option, index):
        if index.column() == 1:  # Столбец "Значение"
            editor = QDoubleSpinBox(parent)
            editor.setMinimum(-999999)
            editor.setMaximum(999999)
            editor.setDecimals(2)
            return editor
        return super().createEditor(parent, option, index)

    def setEditorData(self, editor, index):
        if isinstance(editor, QDoubleSpinBox):
            try:
                value = float(index.model().data(index, Qt.EditRole))
                editor.setValue(value)
            except (ValueError, TypeError):
                editor.setValue(0.0)
        else:
            super().setEditorData(editor, index)

    def setModelData(self, editor, model, index):
        if isinstance(editor, QDoubleSpinBox):
            model.setData(index, editor.value(), Qt.EditRole)
        else:
            super().setModelData(editor, model, index)


class PostgresApp(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Система управления объектами и характеристиками")
        self.resize(1200, 700)

        # 1. Подключение к БД PostgreSQL
        self.db = QSqlDatabase.addDatabase("QPSQL")
        self.db.setHostName("localhost")
        self.db.setDatabaseName("mydb")
        self.db.setUserName("postgres")
        self.db.setPassword("postgres")
        self.db.setPort(5432)

        if not self.db.open():
            QMessageBox.critical(self, "Ошибка",
                                 f"Не удалось подключиться к БД: {self.db.lastError().text()}")
            sys.exit(1)

        self.current_transmitter_id = None
        self.init_ui()
        self.load_tree_data()

    def init_ui(self):
        """Инициализация пользовательского интерфейса"""
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QHBoxLayout(central_widget)

        # --- ЛЕВАЯ ЧАСТЬ: Дерево объектов (Tree Model) ---
        left_panel = QVBoxLayout()

        left_panel.addWidget(self._create_label("Навигация по объектам"))

        self.tree_widget = QTreeWidget()
        self.tree_widget.setHeaderLabels(["Объекты", "Координаты"])
        self.tree_widget.itemSelectionChanged.connect(self.on_tree_selection_changed)
        left_panel.addWidget(self.tree_widget)

        # Кнопки управления деревом
        btn_layout = QVBoxLayout()

        btn_add_object = QPushButton("Добавить объект")
        btn_add_object.clicked.connect(self.add_object)
        btn_layout.addWidget(btn_add_object)

        btn_add_transmitter = QPushButton("Добавить передачу")
        btn_add_transmitter.clicked.connect(self.add_transmitter)
        btn_layout.addWidget(btn_add_transmitter)

        btn_delete = QPushButton("Удалить элемент")
        btn_delete.clicked.connect(self.delete_element)
        btn_layout.addWidget(btn_delete)

        left_panel.addLayout(btn_layout)

        # --- ПРАВАЯ ЧАСТЬ: Таблица ТТХ ---
        right_panel = QVBoxLayout()

        right_panel.addWidget(self._create_label("Технические характеристики"))

        self.param_table = QTableWidget(0, 2)
        self.param_table.setHorizontalHeaderLabels(["Параметр", "Значение"])
        self.param_table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        self.param_table.setItemDelegate(NumericDelegate())
        self.param_table.itemChanged.connect(self.on_table_item_changed)
        right_panel.addWidget(self.param_table)

        # Компоновка
        main_layout.addLayout(left_panel, 1)
        main_layout.addLayout(right_panel, 1)

    def _create_label(self, text):
        """Вспомогательный метод для создания заголовков"""
        from PySide6.QtWidgets import QLabel
        label = QLabel(text)
        label.setStyleSheet("font-weight: bold; font-size: 12px;")
        return label

    def load_tree_data(self):
        """Загрузка данных из БД в дерево"""
        self.tree_widget.clear()

        query = QSqlQuery(self.db)
        query.exec("SELECT id, name, latitude, longitude FROM objects ORDER BY name")

        while query.next():
            obj_id = query.value(0)
            obj_name = query.value(1)
            latitude = query.value(2)
            longitude = query.value(3)

            # Создание узла объекта
            obj_item = QTreeWidgetItem()
            obj_item.setText(0, obj_name)
            obj_item.setText(1, f"({latitude}, {longitude})")
            obj_item.setData(0, Qt.UserRole, ("object", obj_id))
            self.tree_widget.addTopLevelItem(obj_item)

            # Загрузка передатчиков для этого объекта
            tx_query = QSqlQuery(self.db)
            tx_query.exec(f"SELECT id, name FROM transmitters WHERE object_id = {obj_id} ORDER BY name")

            while tx_query.next():
                tx_id = tx_query.value(0)
                tx_name = tx_query.value(1)

                # Создание узла передатчика
                tx_item = QTreeWidgetItem(obj_item)
                tx_item.setText(0, tx_name)
                tx_item.setData(0, Qt.UserRole, ("transmitter", tx_id))

                # Загрузка ТТХ для этого передатчика
                specs_query = QSqlQuery(self.db)
                specs_query.exec(f"SELECT id FROM specs WHERE transmitter_id = {tx_id}")

                if specs_query.next():
                    specs_id = specs_query.value(0)
                    specs_item = QTreeWidgetItem(tx_item)
                    specs_item.setText(0, "ТТХ")
                    specs_item.setData(0, Qt.UserRole, ("specs", specs_id, tx_id))

    def on_tree_selection_changed(self):
        """Обработчик изменения выбора в дереве"""
        selected_items = self.tree_widget.selectedItems()
        if not selected_items:
            self.param_table.setRowCount(0)
            self.current_transmitter_id = None
            return

        item = selected_items[0]
        data = item.data(0, Qt.UserRole)

        if data and len(data) >= 2:
            item_type = data[0]

            if item_type == "transmitter":
                self.current_transmitter_id = data[1]
                self.load_specs_table(data[1])
            elif item_type == "specs":
                self.current_transmitter_id = data[2]
                self.load_specs_table(data[2])
            else:
                self.param_table.setRowCount(0)
                self.current_transmitter_id = None
        else:
            self.param_table.setRowCount(0)
            self.current_transmitter_id = None

    def load_specs_table(self, transmitter_id):
        """Загрузка ТТХ в таблицу для выбранного передатчика"""
        self.param_table.setRowCount(0)

        query = QSqlQuery(self.db)
        query.exec(f"""
            SELECT power_watt, gain_db, antenna_height 
            FROM specs 
            WHERE transmitter_id = {transmitter_id}
        """)

        if query.next():
            specs = [
                ("Мощность (Вт)", query.value(0)),
                ("КУ (дБ)", query.value(1)),
                ("Высота подвеса (м)", query.value(2))
            ]

            self.param_table.setRowCount(len(specs))

            for row, (param_name, param_value) in enumerate(specs):
                param_item = QTableWidgetItem(param_name)
                param_item.setFlags(param_item.flags() & ~Qt.ItemIsEditable)
                self.param_table.setItem(row, 0, param_item)

                value_item = QTableWidgetItem(str(param_value) if param_value else "0")
                self.param_table.setItem(row, 1, value_item)

    def on_table_item_changed(self, item):
        """Обработчик изменения значения в таблице"""
        if item.column() == 1 and self.current_transmitter_id:
            row = item.row()
            param_name = self.param_table.item(row, 0).text()

            try:
                new_value = float(item.text())
            except ValueError:
                QMessageBox.warning(self, "Ошибка", "Введите корректное числовое значение")
                return

            # Определение поля для обновления
            field_map = {
                "Мощность (Вт)": "power_watt",
                "КУ (дБ)": "gain_db",
                "Высота подвеса (м)": "antenna_height"
            }

            if param_name in field_map:
                field = field_map[param_name]
                query = QSqlQuery(self.db)
                query.exec(f"""
                    UPDATE specs 
                    SET {field} = {new_value} 
                    WHERE transmitter_id = {self.current_transmitter_id}
                """)

                if not query.isActive():
                    QMessageBox.critical(self, "Ошибка",
                                         f"Не удалось обновить данные: {query.lastError().text()}")

    def add_object(self):
        """Добавление нового объекта"""
        name, ok = QInputDialog.getText(self, "Добавить объект", "Введите имя объекта:")
        if ok and name:
            lat, ok = QInputDialog.getDouble(self, "Координаты", "Широта:", 0.0, -90.0, 90.0, 4)
            if ok:
                lon, ok = QInputDialog.getDouble(self, "Координаты", "Долгота:", 0.0, -180.0, 180.0, 4)
                if ok:
                    query = QSqlQuery(self.db)
                    query.exec(f"""
                        INSERT INTO objects (name, latitude, longitude) 
                        VALUES ('{name}', {lat}, {lon})
                    """)

                    if query.isActive():
                        self.load_tree_data()
                    else:
                        QMessageBox.critical(self, "Ошибка",
                                             f"Не удалось добавить объект: {query.lastError().text()}")

    def add_transmitter(self):
        """Добавление новой передачи"""
        selected_items = self.tree_widget.selectedItems()
        if not selected_items:
            QMessageBox.warning(self, "Ошибка", "Выберите объект для добавления передачи")
            return

        item = selected_items[0]
        data = item.data(0, Qt.UserRole)

        if not data or data[0] != "object":
            QMessageBox.warning(self, "Ошибка", "Выберите объект (не передачу)")
            return

        obj_id = data[1]
        name, ok = QInputDialog.getText(self, "Добавить передачу", "Введите имя передачи:")

        if ok and name:
            query = QSqlQuery(self.db)
            query.exec(f"""
                INSERT INTO transmitters (object_id, name) 
                VALUES ({obj_id}, '{name}')
            """)

            if query.isActive():
                # Получаем ID новой передачи
                last_id_query = QSqlQuery(self.db)
                last_id_query.exec("SELECT lastval()")
                if last_id_query.next():
                    tx_id = last_id_query.value(0)
                    # Создаем ТТХ по умолчанию
                    specs_query = QSqlQuery(self.db)
                    specs_query.exec(f"""
                        INSERT INTO specs (transmitter_id, power_watt, gain_db, antenna_height) 
                        VALUES ({tx_id}, 0.0, 0.0, 0.0)
                    """)

                self.load_tree_data()
            else:
                QMessageBox.critical(self, "Ошибка",
                                     f"Не удалось добавить передачу: {query.lastError().text()}")

    def delete_element(self):
        """Удаление выбранного элемента"""
        selected_items = self.tree_widget.selectedItems()
        if not selected_items:
            QMessageBox.warning(self, "Ошибка", "Выберите элемент для удаления")
            return

        item = selected_items[0]
        data = item.data(0, Qt.UserRole)

        if not data:
            return

        item_type = data[0]
        item_id = data[1]

        reply = QMessageBox.question(self, "Подтверждение",
                                     f"Вы уверены, что хотите удалить этот элемент?",
                                     QMessageBox.Yes | QMessageBox.No)

        if reply == QMessageBox.Yes:
            query = QSqlQuery(self.db)

            if item_type == "object":
                query.exec(f"DELETE FROM objects WHERE id = {item_id}")
            elif item_type == "transmitter":
                query.exec(f"DELETE FROM transmitters WHERE id = {item_id}")

            if query.isActive():
                self.load_tree_data()
            else:
                QMessageBox.critical(self, "Ошибка",
                                     f"Не удалось удалить элемент: {query.lastError().text()}")


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = PostgresApp()
    window.show()
    sys.exit(app.exec())
