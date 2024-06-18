# How to add new Cell Layer(outdated)

## [1] Create new class

- Derive new class from `CellLayer` in `src/widgets/cell/cellLayer.h`

<details style="margin-left: 5rem;">

<summary>Show example</summary>

``` cpp
class Background : public CellLayer {
   public:
    static const Background* getBackground(const QString& name) {
        for (Background* i : k_backgrounds_) {
            if (i->name_ == name) {
                return i;
            }
        }

        return nullptr;
    };

    static const QList<Background*> list() { return k_backgrounds_; };

   protected:
    Background(QString name, QString file_path)
        : CellLayer{std::move(name), std::move(file_path)} {
        k_backgrounds_.push_back(this);
    }
    Background() : CellLayer("No Background") {
        k_backgrounds_.push_back(this);
    }

    inline static QList<Background*> k_backgrounds_{};
    inline static const QString kJsonName{"Background"};

    friend void loadCellLayersFromJson();
};
```

</details>

- Update `loadCellLayersFromJson()`

<details style="margin-left: 5rem;">

<summary>Show example</summary>

``` cpp
inline void loadCellLayersFromJson() {
    // ...

    // Background
            new Background{};
            for (QJsonValue i : json[Background::kJsonName].toArray()) {
                QString name{i[Settings::JSON::CellLayer::kNameKey].toString()};
                QString file_name{
                    i[Settings::JSON::CellLayer::kFileKey].toString()};

                if (name.isEmpty() || file_name.isEmpty()) {
                    continue;
                }
                QFile file{Settings::kCellLayerResourcesDirPath + file_name};
                if (!file.exists()) {
                    continue;
                }

                new Background{name, file.fileName()};
            }

    // ...
}
```

</details>

## [2] Update `Cell` class

- Add pointer to your layer as `Cell` new member variable

<details style="margin-left: 5rem;">

<summary>Show example</summary>

``` cpp
class Cell : public QLabel {
    // ...

    private:
    const Background* background_{nullptr};

    // ...
}
```

</details>

- Overload `Cell::setLayer()`

<details style="margin-left: 5rem;">

<summary>Show example</summary>

``` cpp
class Cell : public QLabel {
    // ...

    private:
    void setLayer(const Background* background) { background_ = background; }

    // ...
}
```

</details>

- Update `Cell::drawCell()`

<details style="margin-left: 5rem;">

<summary>Show example</summary>

``` cpp
class Cell : public QLabel {
    // ...

    private:
    void drawCell() {
        // Background
        if (background_ == nullptr || background_->isEmpty()) {
            if (k_use_default_background_ && k_default_background_ != nullptr)
                *pixmap_ = k_default_background_->scaled(size_, size_);
            else
                pixmap_->fill(background_color_);
        } else {
            *pixmap_ = background_->pixmap()->scaled(size_, size_);
        }

        // ...
    }

    // ...
}
```

</details>

## [3] Create new tool

- Add new enum entry in `Tool::ToolType`

<details style="margin-left: 5rem;">

<summary>Show example</summary>

``` cpp
class Tool{
    // ...

    enum ToolType {
        // ...
        kBackground,
        // Add new entries here
        kEnumLength,
    };

    // ...
}
```

</details>

- Overload `Tool::setTool()`

<details style="margin-left: 5rem;">

<summary>Show example</summary>

``` cpp
class Tool{
    // ...

    static void setTool(const Background* background) {
        tool_type_ = kBackground;
        cell_layer_ = background;
    }

    // ...
}
```

</details>

- Update `Cell::onClicked()`

<details style="margin-left: 5rem;">

<summary>Show example</summary>

``` cpp
class Cell : public QLabel {
    // ...

    private slots:
    void onClicked() {
        switch (Tool::toolType()) {
            // ...

            case Tool::kBackground:
                setLayer(static_cast<const Background*>(Tool::cell_layer()));
                drawCell();
                break;
            case Tool::kNone:
            case Tool::kEnumLength:
                break;
        }
    }

    // ...
}
```

</details>

## [OPTIONAL] Update `Cell::clearLayers()`

If you want your new layer to be cleared by **Clear tool**:

- Update `Cell::clearLayers()`

<details style="margin-left: 5rem;">

<summary>Show example</summary>

``` cpp
class Cell : public QLabel {
    // ...

    private:
    void clearLayers() {
        // ...

        background_ = nullptr;
    }

    // ...
}
```

</details>
