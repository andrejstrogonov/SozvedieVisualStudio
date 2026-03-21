classDiagram
    %% Базовые классы и интерфейсы
    class QObject {
        <<abstract>>
        +deleteLater()
        +objectName()
        +setObjectName()
    }
    
    class QAbstractItemModel {
        <<abstract>>
        +index()
        +parent()
        +data()
        +setData()
        +rowCount()
        +columnCount()
    }
    
    class QMLComponent {
        <<abstract>>
        +create()
    }
    
    %% Модель данных
    class DataModel {
        +rootObject: ObjectNode
        +addObject(name: string, coords: Coordinates, icon: string)
        +addTransmission(objectId: int, name: string, icon: string)
        +addTechnicalParams(transmissionId: int, params: TechnicalParams)
        +getObjectById(id: int)
        +getTransmissionById(id: int)
        +getTechnicalParamsById(id: int)
    }
    
    class ObjectNode {
        +id: int
        +name: string
        +coordinates: Coordinates
        +icon: string
        +children: List~TransmissionNode~
        +parentId: int
    }
    
    class TransmissionNode {
        +id: int
        +name: string
        +icon: string
        +technicalParams: TechnicalParams
        +parentId: int
    }
    
    class TechnicalParams {
        +id: int
        +power: double
        +gain: double
        +height: double
    }
    
    class Coordinates {
        +x: double
        +y: double
    }
    
    %% Модель представления
    class TreeModel {
        +rootNode: ObjectNode
        +columnCount()
        +data(index: QModelIndex, role: int)
        +setData(index: QModelIndex, value: QVariant, role: int)
        +index(row: int, column: int, parent: QModelIndex)
        +parent(index: QModelIndex)
        +rowCount(parent: QModelIndex)
    }
    
    class TreeItem {
        +data(column: int)
        +setData(column: int, value: QVariant)
        +parentItem()
        +childItem(row: int)
        +childCount()
        +row()
        +isLeaf()
    }
    
    %% Представления (QML)
    class TreeView {
        +model: TreeModel
        +delegate: QMLComponent
        +expand()
        +collapse()
    }
    
    class TableView {
        +model: QAbstractItemModel
        +delegate: QMLComponent
        +edit()
        +selection()
    }
    
    %% Делегаты
    class Delegate {
        <<abstract>>
        +createContent()
        +updateContent()
    }
    
    class ObjectDelegate {
        +nameEdit: TextField
        +coordsInput: CoordinatesInput
        +iconPicker: IconButton
    }
    
    class TransmissionDelegate {
        +nameEdit: TextField
        +iconPicker: IconButton
    }
    
    class TechnicalParamsDelegate {
        +powerInput: NumberInput
        +gainInput: NumberInput
        +heightInput: NumberInput
    }
    
    %% Управление данными
    class DatabaseManager {
        +connection: QSqlDatabase
        +insertObject(object: ObjectNode)
        +insertTransmission(transmission: TransmissionNode)
        +insertTechnicalParams(params: TechnicalParams)
        +updateObject(object: ObjectNode)
        +updateTransmission(transmission: TransmissionNode)
        +updateTechnicalParams(params: TechnicalParams)
        +deleteObject(id: int)
        +deleteTransmission(id: int)
        +deleteTechnicalParams(id: int)
        +loadObjects()
        +loadTransmissions()
        +loadTechnicalParams()
    }
    
    %% QML компонент
