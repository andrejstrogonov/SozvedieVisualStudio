import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: specTableModel

    property var specs: []
    property int transmitterId: -1

    function setTransmitterId(id) {
        specs = []
        if (id !== -1) {
            // Здесь должна быть логика для загрузки данных из базы
            specs = [
                { name: "Мощность (Вт)", value: 0.0, fieldName: "power_watt" },
                { name: "КУ (дБ)", value: 0.0, fieldName: "gain_db" },
                { name: "Высота подвеса (м)", value: 0.0, fieldName: "antenna_height" }
            ]
        }
    }

    // Реализация интерфейса QAbstractTableModel
    // Добавьте методы для rowCount, columnCount, data, etc.
}
