#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QWidget>
#include <QScrollArea>
#include <QGridLayout>
#include <QComboBox>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QStackedWidget>
#include <QTreeWidget>
#include <QLineEdit>
#include <QStringList>
#include <QFile>
#include <QDebug>
#include <QFileInfo>
#include <QDir> 
#include <QImageReader>
#include <QStandardPaths>

#include <jsoncpp/json/json.h>

#include "Videos.hpp"
#include "Pelis.hpp"
#include "Serie.hpp"
#include "Cap.hpp"

#include <fstream>
#include <iomanip>
#include <sstream>


Q_DECLARE_METATYPE(Cap*)

QString loadImagePath(const QString& baseName, const QString& resourceDir) {
    QStringList extensions = {".webp", ".png", ".jpg", ".jpeg"};
    for (const auto& ext : extensions) {
        QString tryPath = resourceDir + "/portadas/" + baseName + ext;
        if (QFile::exists(tryPath)) {
            QImageReader reader(tryPath);
            if (reader.canRead()) {
                qDebug() << "Imagen válida encontrada:" << tryPath;
                qDebug() << "Formato:" << reader.format();
                qDebug() << "Tamaño:" << reader.size();
                return tryPath;
            } else {
                qDebug() << "Imagen encontrada pero no se puede leer:" << tryPath;
                qDebug() << "Error:" << reader.errorString();
            }
        }
        qDebug() << "Imagen no encontrada:" << tryPath;
    }
    qDebug() << "No se encontró ninguna imagen válida para:" << baseName;
    return QString();
}

void CrearObjetos(const std::vector<std::string>& claves, const Json::Value& actualJson, 
                  std::vector<Pelis>& peliculas, std::vector<Serie>& series) {
    for (const auto& clave : claves) {
        try {
            std::string nombre, genero, descripcion;
            std::vector<double> calificaciones;
            int horas = 0, minutos = 0, segundos = 0;
            int id_peli = 0, id_serie = 0, id_temp = 0, id_cap = 0;

            uint32_t id = std::stoul(clave, nullptr, 16);

            if (((id >> 16) & 0xFF) == 255 && ((id >> 8) & 0xFF) == 0) {
                id_peli = (id & 0xFF);
                nombre = actualJson[clave]["titulo"].asString();
                genero = actualJson[clave]["genero"].asString();
                horas = actualJson[clave]["duracion"]["horas"].asInt();
                minutos = actualJson[clave]["duracion"]["minutos"].asInt();
                segundos = actualJson[clave]["duracion"]["segundos"].asInt();
                descripcion = actualJson[clave]["descripcion"].asString();
                for (const auto& calif : actualJson[clave]["calificaciones"]) {
                    calificaciones.push_back(calif.asDouble());
                }
                Pelis pelicula(id, nombre, horas, minutos, segundos, genero, calificaciones, id_peli, descripcion);
                peliculas.push_back(pelicula);

            } else if (((id >> 16) & 0xFF) > 0  && ((id >> 8) & 0xFF) > 0 && (id & 0xFF) > 0){
                id_serie = (id >> 16) & 0xFF;
                id_temp = (id >> 8) & 0xFF;
                id_cap = id & 0xFF;
                nombre = actualJson[clave]["titulo"].asString();
                horas = actualJson[clave]["duracion"]["horas"].asInt();
                minutos = actualJson[clave]["duracion"]["minutos"].asInt();
                segundos = actualJson[clave]["duracion"]["segundos"].asInt();
                descripcion = actualJson[clave]["descripcion"].asString();
                for (const auto& calif : actualJson[clave]["calificaciones"]) {
                    calificaciones.push_back(calif.asDouble());
                }
                Cap capitulo(id, nombre, horas, minutos, segundos, "", calificaciones, descripcion, id_serie, id_temp, id_cap);
                if (series.empty() || series.back().getIdSerie() != id_serie) {
                    std::vector<Cap> capitulos;
                    capitulos.push_back(capitulo);
                    Serie serie(id, nombre, id_serie, "", descripcion, calificaciones, capitulos);
                    series.push_back(serie);
                } else {
                    series.back().capitulos.push_back(capitulo);
                }
            } else if (((id >> 16) & 0xFF) > 0  && ((id >> 8) & 0xFF) == 0 && (id & 0xFF) == 0) {
                id_serie = (id >> 16) & 0xFF;
                nombre = actualJson[clave]["titulo"].asString();
                genero = actualJson[clave]["genero"].asString();
                descripcion = actualJson[clave]["descripcion"].asString();
                for (const auto& calif : actualJson[clave]["calificaciones"]) {
                    calificaciones.push_back(calif.asDouble());
                }
                Serie serie(id, nombre, id_serie, genero, descripcion, calificaciones, {});
                series.push_back(serie);
            }
        } catch (...) {
            std::cout << "Error al procesar clave: " << clave << std::endl;
        }
    }
}

void guardarJSON(const std::vector<Pelis>& peliculas, const std::vector<Serie>& series, QMainWindow* window) {
    std::ifstream archivo_lectura("Contenido.json");
    Json::Value root;
    if (archivo_lectura.is_open()) {
        Json::CharReaderBuilder builder;
        std::string errores;
        if (!Json::parseFromStream(builder, archivo_lectura, &root, &errores)) {
            QMessageBox::critical(window, "Error", QString::fromStdString("Error al parsear JSON para guardar: " + errores));
            archivo_lectura.close();
            return;
        }
        archivo_lectura.close();
    } else {
        QMessageBox::critical(window, "Error", "No se pudo abrir Contenido.json para lectura");
        return;
    }

    for (const auto& pelicula : peliculas) {
        std::stringstream ss;
        ss << "0x" << std::hex << std::setw(6) << std::setfill('0') << std::uppercase << pelicula.getId();
        std::string clave = ss.str();
        if (root.isMember(clave)) {
            root[clave]["calificaciones"].clear();
            for (const auto& calif : pelicula.getCalificaciones()) {
                root[clave]["calificaciones"].append(calif);
            }
        }
    }

    for (const auto& serie : series) {
        for (const auto& capitulo : serie.capitulos) {
            std::stringstream ss;
            ss << "0x" << std::hex << std::setw(6) << std::setfill('0') << std::uppercase << capitulo.getId();
            std::string clave = ss.str();
            if (root.isMember(clave)) {
                root[clave]["calificaciones"].clear();
                for (const auto& calif : capitulo.getCalificaciones()) {
                    root[clave]["calificaciones"].append(calif);
                }
            }
        }
    }

    std::ofstream archivo("Contenido.json");
    if (!archivo.is_open()) {
        QMessageBox::critical(window, "Error", "No se pudo abrir Contenido.json para escritura");
        return;
    }
    archivo << root.toStyledString();
    archivo.close();
    QMessageBox::information(window, "SI JALA", "Se guardo el json sin arderse");

}

double calcularPromedio(const std::vector<double>& calificaciones) {
    if (calificaciones.empty()) return 0.0;
    double suma = 0.0;
    for (const auto& calif : calificaciones) {
        suma += calif;
    }
    return suma / calificaciones.size();
}

double calcularPromedioSerie(const Serie& serie) {
    std::vector<double> promediosCapitulos;
    for (const auto& capitulo : serie.capitulos) {
        auto califs = capitulo.getCalificaciones();
        promediosCapitulos.push_back(calcularPromedio(califs));
    }
    return calcularPromedio(promediosCapitulos);
}

// Función para quitar acentos de un QString (más robusta)
QString quitarAcentos(const QString& texto) {
    QString resultado;
    for (QChar c : texto) {
        ushort u = c.unicode();
        // Letras acentuadas comunes en español
        switch (u) {
            case 0x00E1: case 0x00C1: resultado += 'a'; break; // á Á
            case 0x00E9: case 0x00C9: resultado += 'e'; break; // é É
            case 0x00ED: case 0x00CD: resultado += 'i'; break; // í Í
            case 0x00F3: case 0x00D3: resultado += 'o'; break; // ó Ó
            case 0x00FA: case 0x00DA: resultado += 'u'; break; // ú Ú
            case 0x00FC: case 0x00DC: resultado += 'u'; break; // ü Ü
            case 0x00F1: case 0x00D1: resultado += 'n'; break; // ñ Ñ
            default:
                if (c.category() != QChar::Mark_NonSpacing)
                    resultado += c;
        }
    }
    return resultado;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QMainWindow window;
    window.setWindowTitle("Tenflix");
    window.resize(800, 600);

    // Set resource directory
    QString resourceDir = QCoreApplication::applicationDirPath();
    qDebug() << "Directorio de recursos:" << resourceDir;

    // Cargar datos
    std::vector<Pelis> peliculas;
    std::vector<Serie> series;
    Json::Value actualJson;

    std::ifstream archivo("./Contenido.json");
    if (!archivo.is_open()) {
        QMessageBox::critical(&window, "Error", "No se pudo abrir Contenido.json");
        return 1;
    }

    Json::CharReaderBuilder builder;
    std::string errores;
    if (!Json::parseFromStream(builder, archivo, &actualJson, &errores)) {
        QMessageBox::critical(&window, "Error", QString::fromStdString("Error al parsear JSON: " + errores));
        archivo.close();
        return 1;
    }
    archivo.close();

    CrearObjetos(actualJson.getMemberNames(), actualJson, peliculas, series);

    // Crear QStackedWidget para cambiar entre vistas
    QStackedWidget *stackedWidget = new QStackedWidget(&window);

    // Vista principal (películas y series)
    QWidget *mainView = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(mainView);

    // --- NUEVO: Widgets de búsqueda ---
    QHBoxLayout *searchLayout = new QHBoxLayout();
    QLineEdit *searchInput = new QLineEdit();
    searchInput->setPlaceholderText("Buscar por título, descripción o género...");
    QPushButton *searchButton = new QPushButton("Buscar");
    searchLayout->addWidget(searchInput);
    searchLayout->addWidget(searchButton);
    mainLayout->insertLayout(0, searchLayout);

    QString lastSearch = "";

    QPushButton *backButtonMain = new QPushButton("Regresar", mainView);
    backButtonMain->setVisible(false); // Oculto en la vista principal
    mainLayout->addWidget(backButtonMain);

    QHBoxLayout *navLayout = new QHBoxLayout();
    QPushButton *btnPeliculas = new QPushButton("Películas");
    QPushButton *btnSeries = new QPushButton("Series");
    QPushButton *btnTodo = new QPushButton("Todo");
    navLayout->addWidget(btnPeliculas);
    navLayout->addWidget(btnSeries);
    navLayout->addWidget(btnTodo);
    navLayout->addStretch();
    mainLayout->addLayout(navLayout);

    QScrollArea *scrollArea = new QScrollArea();
    QWidget *contentWidget = new QWidget();
    QGridLayout *contentLayout = new QGridLayout(contentWidget);
    scrollArea->setWidget(contentWidget);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);

    stackedWidget->addWidget(mainView);

    // Declarar todas las funciones lambda antes de implementarlas
    std::function<void()> mostrarPeliculas;
    std::function<void()> mostrarSeries;
    std::function<void(const QString &)> mostrarPeliculasFiltrado;
    std::function<void(const QString &)> mostrarSeriesFiltrado;
    std::function<void(const QString &)> mostrarTodoFiltrado;

    // Función para mostrar películas
    mostrarPeliculas = [&]() {
    QLayoutItem *child;
    while ((child = contentLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    int row = 0, col = 0;
    for (const auto& pelicula : peliculas) {
        QWidget *itemWidget = new QWidget();
        QHBoxLayout *itemLayout = new QHBoxLayout(itemWidget);
        itemLayout->setContentsMargins(0, 0, 0, 0);
        itemLayout->setSpacing(10); // Espacio entre imagen y texto

        // Imagen (columna izquierda)
        QString baseName = QString::fromStdString(pelicula.getNombre());
        QString imagePath = loadImagePath(baseName, resourceDir);
        QPixmap pixmap;
        QLabel *imageLabel = new QLabel();
        if (!imagePath.isEmpty() && pixmap.load(imagePath)) {
            imageLabel->setPixmap(pixmap.scaled(140, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            imageLabel->setText("Sin imagen");
            imageLabel->setAlignment(Qt::AlignCenter);
        }
        imageLabel->setFixedSize(140, 180);
        itemLayout->addWidget(imageLabel);

        // Texto (columna derecha)mostra
        double promedio = calcularPromedio(pelicula.getCalificaciones());
        QLabel *textLabel = new QLabel(
            QString("%1\n★ %2").arg(baseName).arg(QString::number(promedio, 'f', 1)));
        textLabel->setAlignment(Qt::AlignCenter);
        textLabel->setStyleSheet("font-size: 12px; padding: 10px;");
        itemLayout->addWidget(textLabel);

        // Botón para acciones (opcional, puede ser un QPushButton separado)
        QPushButton *actionButton = new QPushButton("Ver");
        actionButton->setFixedSize(50, 180);
        itemLayout->addWidget(actionButton);

        itemWidget->setLayout(itemLayout);
        contentLayout->addWidget(itemWidget, row, col);
        col++;
        if (col >= 4) {
            col = 0;
            row++;
        }

        // Vista detallada (movieView)
        QWidget *movieView = new QWidget();
        QVBoxLayout *movieLayout = new QVBoxLayout(movieView);
        QPushButton *backButton = new QPushButton("Regresar");
        movieLayout->addWidget(backButton);

        QHBoxLayout *topLayout = new QHBoxLayout();
        QLabel *poster = new QLabel();
        if (!pixmap.isNull()) {
            poster->setPixmap(pixmap.scaled(300, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            poster->setAlignment(Qt::AlignCenter);
        } else {
            poster->setText("Sin imagen");
            poster->setAlignment(Qt::AlignCenter);
        }
        topLayout->addWidget(poster);

        QVBoxLayout *infoLayout = new QVBoxLayout();
        QLabel *info = new QLabel(
            QString("ID: 0x%1\nPelícula ID: %2\nNombre: %3\nDuración: %4h %5m %6s\nGénero: %7\nDescripción: %8\nCalificación: ★ %9")
                .arg(pelicula.getId(), 0, 16)
                .arg(pelicula.getIdPeli())
                .arg(QString::fromStdString(pelicula.getNombre()))
                .arg(pelicula.getHoras())
                .arg(pelicula.getMinutos())
                .arg(pelicula.getSegundos())
                .arg(QString::fromStdString(pelicula.getGenero()))
                .arg(QString::fromStdString(pelicula.getDescripcion()))
                .arg(QString::number(promedio, 'f', 1)));
        info->setWordWrap(true);
        infoLayout->addWidget(info);
        topLayout->addLayout(infoLayout);
        movieLayout->addLayout(topLayout);

        QVideoWidget *videoWidget = new QVideoWidget();
        videoWidget->setFixedSize(600, 400);
        QMediaPlayer *player = new QMediaPlayer(movieView);
        player->setVideoOutput(videoWidget);
        QString videoPath = resourceDir + "/videos/" + QString::fromStdString(pelicula.getNombre()) + ".mp4";
        if (QFile::exists(videoPath)) {
            player->setMedia(QUrl::fromLocalFile(QFileInfo(videoPath).absoluteFilePath()));
        } else {
            qDebug() << "Video no encontrado:" << videoPath;
        }
        movieLayout->addWidget(videoWidget);

        QPushButton *pauseButton = new QPushButton("Pausar");
        movieLayout->addWidget(pauseButton);
        QObject::connect(pauseButton, &QPushButton::clicked, [player, pauseButton]() {
            if (player->state() == QMediaPlayer::PlayingState) {
                player->pause();
                pauseButton->setText("Reanudar");
            } else if (player->state() == QMediaPlayer::PausedState) {
                player->play();
                pauseButton->setText("Pausar");
            }
        });

        QHBoxLayout *ratingLayout = new QHBoxLayout();
        QComboBox *ratingCombo = new QComboBox();
        ratingCombo->addItems({"1", "2", "3", "4", "5"});
        QPushButton *submitRating = new QPushButton("Enviar");
        ratingLayout->addWidget(new QLabel("Calificar:"));
        ratingLayout->addWidget(ratingCombo);
        ratingLayout->addWidget(submitRating);
        movieLayout->addLayout(ratingLayout);

        stackedWidget->addWidget(movieView);

        QObject::connect(actionButton, &QPushButton::clicked, [stackedWidget, movieView, player, videoPath]() {
            stackedWidget->setCurrentWidget(movieView);
            if (QFile::exists(videoPath)) {
                player->setMedia(QUrl::fromLocalFile(QFileInfo(videoPath).absoluteFilePath()));
                player->play();
            }
        });
        QObject::connect(backButton, &QPushButton::clicked, [stackedWidget, mainView, player, &mostrarPeliculas]() {
            player->stop();
            stackedWidget->setCurrentWidget(mainView);
            mostrarPeliculas();
        });
        QObject::connect(submitRating, &QPushButton::clicked, [ratingCombo, &pelicula, info, &peliculas, &series, &window]() {
            double rating = ratingCombo->currentText().toDouble();
            const_cast<Pelis&>(pelicula).setCalificaciones(rating);
            guardarJSON(peliculas, series, &window);
            QMessageBox::information(nullptr, "Éxito", "Calificación agregada.");
            double nuevoPromedio = calcularPromedio(pelicula.getCalificaciones());
            info->setText(info->text().replace(QRegularExpression("Calificación: ★ [0-9.]+"),
                                               QString("Calificación: ★ %1").arg(QString::number(nuevoPromedio, 'f', 1))));
        });
    }
};

    // Función para mostrar series
    mostrarSeries = [&]() {
    QLayoutItem *child;
    while ((child = contentLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    int row = 0, col = 0;
    for (const auto& serie : series) {
        QWidget *itemWidget = new QWidget();
        QHBoxLayout *itemLayout = new QHBoxLayout(itemWidget);
        itemLayout->setContentsMargins(0, 0, 0, 0);
        itemLayout->setSpacing(10);

        // Imagen (columna izquierda)
        QString baseName = QString::fromStdString(serie.getNombre());
        QString imagePath = loadImagePath(baseName, resourceDir);
        QPixmap pixmap;
        QLabel *imageLabel = new QLabel();
        if (!imagePath.isEmpty() && pixmap.load(imagePath)) {
            imageLabel->setPixmap(pixmap.scaled(140, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            imageLabel->setText("Sin imagen");
            imageLabel->setAlignment(Qt::AlignCenter);
        }
        imageLabel->setFixedSize(140, 180);
        itemLayout->addWidget(imageLabel);

        // Texto (columna derecha)
        double promedio = calcularPromedioSerie(serie);
        QLabel *textLabel = new QLabel(
            QString("%1\n★ %2").arg(baseName).arg(QString::number(promedio, 'f', 1)));
        textLabel->setAlignment(Qt::AlignCenter);
        textLabel->setStyleSheet("font-size: 12px; padding: 10px;");
        itemLayout->addWidget(textLabel);

        // Botón para acciones
        QPushButton *actionButton = new QPushButton("Ver");
        actionButton->setFixedSize(50, 180);
        itemLayout->addWidget(actionButton);

        itemWidget->setLayout(itemLayout);
        contentLayout->addWidget(itemWidget, row, col);
        col++;
        if (col >= 4) {
            col = 0;
            row++;
        }

        // Vista detallada (serieView y chapterView)
        QWidget *serieView = new QWidget();
        QVBoxLayout *serieLayout = new QVBoxLayout(serieView);
        QPushButton *backButton = new QPushButton("Regresar");
        serieLayout->addWidget(backButton);

        QHBoxLayout *topLayout = new QHBoxLayout();
        QLabel *poster = new QLabel();
        if (!pixmap.isNull()) {
            poster->setPixmap(pixmap.scaled(300, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            poster->setAlignment(Qt::AlignCenter);
        } else {
            poster->setText("Sin imagen");
            poster->setAlignment(Qt::AlignCenter);
        }
        topLayout->addWidget(poster);

        QVBoxLayout *infoLayout = new QVBoxLayout();
        QLabel *info = new QLabel(
            QString("ID: 0x%1\nID Serie: %2\nNombre: %3\nGénero: %4\nDescripción: %5\nCalificación: ★ %6")
                .arg(serie.getId(), 0, 16)
                .arg(serie.getIdSerie())
                .arg(QString::fromStdString(serie.getNombre()))
                .arg(QString::fromStdString(serie.getGenero()))
                .arg(QString::fromStdString(serie.getDescripcion()))
                .arg(QString::number(promedio, 'f', 1)));
        info->setWordWrap(true);
        infoLayout->addWidget(info);
        topLayout->addLayout(infoLayout);
        serieLayout->addLayout(topLayout);

        QTreeWidget *chaptersTree = new QTreeWidget();
        chaptersTree->setHeaderLabel("Temporadas y Capítulos");
        std::map<int, QTreeWidgetItem*> seasons;
        for (const auto& capitulo : serie.capitulos) {
            int season = capitulo.getIdTemp();
            if (seasons.find(season) == seasons.end()) {
                seasons[season] = new QTreeWidgetItem(chaptersTree);
                seasons[season]->setText(0, QString("Temporada %1").arg(season));
            }
            QTreeWidgetItem *chapterItem = new QTreeWidgetItem(seasons[season]);
            chapterItem->setText(0, QString("Capítulo %1 - %2").arg(capitulo.getIdCap()).arg(QString::fromStdString(capitulo.getNombre())));
            chapterItem->setData(0, Qt::UserRole, QVariant::fromValue(const_cast<Cap*>(&capitulo)));
        }
        chaptersTree->expandAll();
        serieLayout->addWidget(chaptersTree);

        stackedWidget->addWidget(serieView);

        QWidget *chapterView = new QWidget();
        QVBoxLayout *chapterLayout = new QVBoxLayout(chapterView);
        QPushButton *chapterBackButton = new QPushButton("Regresar");
        chapterLayout->addWidget(chapterBackButton);

        QHBoxLayout *chapterTopLayout = new QHBoxLayout();
        QLabel *chapterPoster = new QLabel();
        if (!pixmap.isNull()) {
            chapterPoster->setPixmap(pixmap.scaled(300, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            chapterPoster->setAlignment(Qt::AlignCenter);
        } else {
            chapterPoster->setText("Sin imagen");
            chapterPoster->setAlignment(Qt::AlignCenter);
        }
        chapterTopLayout->addWidget(chapterPoster);

        QVBoxLayout *chapterInfoLayout = new QVBoxLayout();
        QLabel *chapterInfo = new QLabel();
        chapterInfo->setWordWrap(true);
        chapterInfoLayout->addWidget(chapterInfo);
        chapterTopLayout->addLayout(chapterInfoLayout);
        chapterLayout->addLayout(chapterTopLayout);

        QVideoWidget *chapterVideoWidget = new QVideoWidget();
        chapterVideoWidget->setFixedSize(600, 400);
        QMediaPlayer *chapterPlayer = new QMediaPlayer(chapterView);
        chapterPlayer->setVideoOutput(chapterVideoWidget);
        chapterLayout->addWidget(chapterVideoWidget);

        QPushButton *pauseButton = new QPushButton("Pausar");
        chapterLayout->addWidget(pauseButton);
        QObject::connect(pauseButton, &QPushButton::clicked, [chapterPlayer, pauseButton]() {
            if (chapterPlayer->state() == QMediaPlayer::PlayingState) {
                chapterPlayer->pause();
                pauseButton->setText("Reanudar");
            } else if (chapterPlayer->state() == QMediaPlayer::PausedState) {
                chapterPlayer->play();
                pauseButton->setText("Pausar");
            }
        });

        QHBoxLayout *chapterRatingLayout = new QHBoxLayout();
        QLabel *chapterRatingLabel = new QLabel("Calificar:");
        QComboBox *chapterRatingCombo = new QComboBox();
        chapterRatingCombo->addItems({"1", "2", "3", "4", "5"});
        QPushButton *chapterSubmitRating = new QPushButton("Enviar");
        chapterRatingLayout->addWidget(chapterRatingLabel);
        chapterRatingLayout->addWidget(chapterRatingCombo);
        chapterRatingLayout->addWidget(chapterSubmitRating);
        chapterLayout->addLayout(chapterRatingLayout);

        stackedWidget->addWidget(chapterView);

        QObject::connect(actionButton, &QPushButton::clicked, [stackedWidget, serieView]() {
            stackedWidget->setCurrentWidget(serieView);
        });
        QObject::connect(backButton, &QPushButton::clicked, [stackedWidget, mainView, &mostrarSeries]() {
            stackedWidget->setCurrentWidget(mainView);
            mostrarSeries();
        });
        QObject::connect(chaptersTree, &QTreeWidget::itemDoubleClicked, [stackedWidget, chapterView, chapterInfo, chapterPlayer, &serie, resourceDir](QTreeWidgetItem *item, int) {
            Cap *capitulo = item->data(0, Qt::UserRole).value<Cap*>();
            if (!capitulo) return;
            QString infoCap = QString("ID: 0x%1\nID Serie: %2\nTemporada: %3\nCapítulo: %4\nNombre: %5\nDuración: %6h %7m %8s\nDescripción: %9\nCalificación: ★ %10")
                .arg(capitulo->getId(), 0, 16)
                .arg(capitulo->getIdSerie())
                .arg(capitulo->getIdTemp())
                .arg(capitulo->getIdCap())
                .arg(QString::fromStdString(capitulo->getNombre()))
                .arg(capitulo->getHoras())
                .arg(capitulo->getMinutos())
                .arg(capitulo->getSegundos())
                .arg(QString::fromStdString(capitulo->getDescripcion()))
                .arg(QString::number(calcularPromedio(capitulo->getCalificaciones()), 'f', 1));
            chapterInfo->setText(infoCap);

            QString videoPath = resourceDir + "/videos/" + QString::fromStdString(capitulo->getNombre()) + ".mp4";
            if (QFile::exists(videoPath)) {
                chapterPlayer->setMedia(QUrl::fromLocalFile(QFileInfo(videoPath).absoluteFilePath()));
                chapterPlayer->play();
            } else {
                qDebug() << "Video no encontrado:" << videoPath;
                chapterPlayer->setMedia(QUrl());
            }
            stackedWidget->setCurrentWidget(chapterView);
        });
        QObject::connect(chapterBackButton, &QPushButton::clicked, [stackedWidget, serieView, chapterPlayer]() {
            chapterPlayer->stop();
            stackedWidget->setCurrentWidget(serieView);
        });
        QObject::connect(chapterSubmitRating, &QPushButton::clicked, [chaptersTree, chapterRatingCombo, chapterInfo, &peliculas, &series, &window]() {
            Cap *capitulo = chaptersTree->currentItem()->data(0, Qt::UserRole).value<Cap*>();
            if (!capitulo) return;
            double rating = chapterRatingCombo->currentText().toDouble();
            capitulo->setCalificaciones(rating);
            guardarJSON(peliculas, series, &window);
            QMessageBox::information(nullptr, "Éxito", "Calificación agregada.");
            chapterInfo->setText(
                QString("ID: 0x%1\nID Serie: %2\nTemporada: %3\nCapítulo: %4\nNombre: %5\nDuración: %6h %7m %8s\nDescripción: %9\nCalificación: ★ %10")
                    .arg(capitulo->getId(), 0, 16)
                    .arg(capitulo->getIdSerie())
                    .arg(capitulo->getIdTemp())
                    .arg(capitulo->getIdCap())
                    .arg(QString::fromStdString(capitulo->getNombre()))
                    .arg(capitulo->getHoras())
                    .arg(capitulo->getMinutos())
                    .arg(capitulo->getSegundos())
                    .arg(QString::fromStdString(capitulo->getDescripcion()))
                    .arg(QString::number(calcularPromedio(capitulo->getCalificaciones()), 'f', 1))
            );
        });
    }
};

    // Función para mostrar películas con filtro
    mostrarPeliculasFiltrado = [&](const QString &filtro) {
        QLayoutItem *child;
        while ((child = contentLayout->takeAt(0)) != nullptr) {
            delete child->widget();
            delete child;
        }
        QString filtroSinAcentos = quitarAcentos(filtro).toLower();
        int row = 0, col = 0;
        for (const auto& pelicula : peliculas) {
            QString titulo = QString::fromStdString(pelicula.getNombre());
            QString descripcion = QString::fromStdString(pelicula.getDescripcion());
            QString genero = QString::fromStdString(pelicula.getGenero());
            QString tituloSinAcentos = quitarAcentos(titulo).toLower();
            QString descripcionSinAcentos = quitarAcentos(descripcion).toLower();
            QString generoSinAcentos = quitarAcentos(genero).toLower();
            if (!filtroSinAcentos.isEmpty() &&
                !tituloSinAcentos.contains(filtroSinAcentos) &&
                !descripcionSinAcentos.contains(filtroSinAcentos) &&
                !generoSinAcentos.contains(filtroSinAcentos)) {
                continue;
            }
            QPushButton *btn = new QPushButton();
            btn->setFixedSize(150, 200);

            // Carga imagen robusta
            QString baseName = titulo;
            QString imagePath = loadImagePath(baseName, resourceDir);
            QPixmap pixmap;
            bool loaded = false;
            if (!imagePath.isEmpty() && pixmap.load(imagePath)) {
                QIcon icon(pixmap.scaled(140, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                btn->setIcon(icon);
                btn->setIconSize(QSize(140, 180));
                loaded = true;
            }
            if (!loaded) {
                btn->setStyleSheet("QPushButton { text-align: bottom; padding-top: 10px; }");
            }

            double promedio = calcularPromedio(pelicula.getCalificaciones());
            btn->setText(QString("%1\n★ %2").arg(baseName).arg(QString::number(promedio, 'f', 1)));
            contentLayout->addWidget(btn, row, col);
            col++;
            if (col >= 4) {
                col = 0;
                row++;
            }

            QWidget *movieView = new QWidget();
            QVBoxLayout *movieLayout = new QVBoxLayout(movieView);
            QPushButton *backButton = new QPushButton("Regresar");
            movieLayout->addWidget(backButton);

            QHBoxLayout *topLayout = new QHBoxLayout();
            QLabel *poster = new QLabel();
            if (loaded) {
                poster->setPixmap(pixmap.scaled(300, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                poster->setVisible(true);
                poster->repaint();
            } else {
                poster->setText("Sin imagen");
            }
            topLayout->addWidget(poster);

            QVBoxLayout *infoLayout = new QVBoxLayout();
            QLabel *info = new QLabel(
                QString("ID: 0x%1\nPelícula ID: %2\nNombre: %3\nDuración: %4h %5m %6s\nGénero: %7\nDescripción: %8\nCalificación: ★ %9")
                    .arg(pelicula.getId(), 0, 16)
                    .arg(pelicula.getIdPeli())
                    .arg(titulo)
                    .arg(pelicula.getHoras())
                    .arg(pelicula.getMinutos())
                    .arg(pelicula.getSegundos())
                    .arg(genero)
                    .arg(descripcion)
                    .arg(QString::number(promedio, 'f', 1)));
            info->setWordWrap(true);
            infoLayout->addWidget(info);
            topLayout->addLayout(infoLayout);
            movieLayout->addLayout(topLayout);

            QVideoWidget *videoWidget = new QVideoWidget();
            videoWidget->setFixedSize(600, 400);
            QMediaPlayer *player = new QMediaPlayer();
            player->setVideoOutput(videoWidget);
            QString videoPath = QDir::currentPath() + "/videos/" + baseName + ".mp4";
            if (QFile::exists(videoPath)) {
                player->setMedia(QUrl::fromLocalFile(QFileInfo(videoPath).absoluteFilePath()));
                player->play();
            } else {
                qDebug() << "Video no encontrado:" << videoPath;
            }
            movieLayout->addWidget(videoWidget);

            // Botón de pausa/play con cambio de texto
            QPushButton *pauseButton = new QPushButton("Pausar");
            movieLayout->addWidget(pauseButton);
            QObject::connect(pauseButton, &QPushButton::clicked, [player, pauseButton]() {
                if (player->state() == QMediaPlayer::PlayingState) {
                    player->pause();
                    pauseButton->setText("Reanudar");
                } else if (player->state() == QMediaPlayer::PausedState) {
                    player->play();
                    pauseButton->setText("Pausar");
                }
            });

            QHBoxLayout *ratingLayout = new QHBoxLayout();
            QComboBox *ratingCombo = new QComboBox();
            ratingCombo->addItems({"1", "2", "3", "4", "5"});
            QPushButton *submitRating = new QPushButton("Enviar");
            ratingLayout->addWidget(new QLabel("Calificar:"));
            ratingLayout->addWidget(ratingCombo);
            ratingLayout->addWidget(submitRating);
            movieLayout->addLayout(ratingLayout);

            stackedWidget->addWidget(movieView);

            QObject::connect(btn, &QPushButton::clicked, [stackedWidget, movieView]() {
                stackedWidget->setCurrentWidget(movieView);
            });
            QObject::connect(backButton, &QPushButton::clicked, [stackedWidget, mainView, player, filtro, &mostrarPeliculasFiltrado]() mutable {
                player->stop();
                stackedWidget->setCurrentWidget(mainView);
                mostrarPeliculasFiltrado(filtro);
            });
            QObject::connect(submitRating, &QPushButton::clicked, [ratingCombo, &pelicula, info, &peliculas, &series, &window]() {
                double rating = ratingCombo->currentText().toDouble();
                const_cast<Pelis&>(pelicula).setCalificaciones(rating);
                guardarJSON(peliculas, series, &window);
                QMessageBox::information(nullptr, "Éxito", "Calificación agregada.");
                double nuevoPromedio = calcularPromedio(pelicula.getCalificaciones());
                info->setText(info->text().replace(QRegularExpression("Calificación: ★ [0-9.]+"),
                                                   QString("Calificación: ★ %1").arg(QString::number(nuevoPromedio, 'f', 1))));
            });
        }
    };

    // Función para mostrar series con filtro
    mostrarSeriesFiltrado = [&](const QString &filtro) {
        // Limpiar el layout anterior
        QLayoutItem *child;
        while ((child = contentLayout->takeAt(0)) != nullptr) {
            delete child->widget();
            delete child;
        }

        QString filtroSinAcentos = quitarAcentos(filtro).toLower();
        int row = 0, col = 0;
        for (const auto& serie : series) {
            QString titulo = QString::fromStdString(serie.getNombre());
            QString descripcion = QString::fromStdString(serie.getDescripcion());
            QString genero = QString::fromStdString(serie.getGenero());
            QString tituloSinAcentos = quitarAcentos(titulo).toLower();
            QString descripcionSinAcentos = quitarAcentos(descripcion).toLower();
            QString generoSinAcentos = quitarAcentos(genero).toLower();
            if (!filtroSinAcentos.isEmpty() &&
                !tituloSinAcentos.contains(filtroSinAcentos) &&
                !descripcionSinAcentos.contains(filtroSinAcentos) &&
                !generoSinAcentos.contains(filtroSinAcentos)) {
                continue;
            }
            QPushButton *btn = new QPushButton();
            btn->setFixedSize(150, 200);

            // Carga imagen robusta
            QString baseName = titulo;
            QString imagePath = loadImagePath(baseName, resourceDir);
            QPixmap pixmap;
            bool loaded = false;
            if (!imagePath.isEmpty() && pixmap.load(imagePath)) {
                QIcon icon(pixmap.scaled(140, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                btn->setIcon(icon);
                btn->setIconSize(QSize(140, 180));
                loaded = true;
            }
            if (!loaded) {
                btn->setStyleSheet("QPushButton { text-align: bottom; padding-top: 10px; }");
            }
            double promedio = calcularPromedioSerie(serie);
            btn->setText(QString("%1\n★ %2").arg(baseName).arg(QString::number(promedio, 'f', 1)));
            contentLayout->addWidget(btn, row, col);
            col++;
            if (col >= 4) {
                col = 0;
                row++;
            }

            QWidget *serieView = new QWidget();
            QVBoxLayout *serieLayout = new QVBoxLayout(serieView);
            QPushButton *backButton = new QPushButton("Regresar");
            serieLayout->addWidget(backButton);

            QHBoxLayout *topLayout = new QHBoxLayout();
            QLabel *poster = new QLabel();
            if (loaded) {
                poster->setPixmap(pixmap.scaled(300, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
            topLayout->addWidget(poster);

            QVBoxLayout *infoLayout = new QVBoxLayout();
            QLabel *info = new QLabel(
                QString("ID: 0x%1\nID Serie: %2\nNombre: %3\nGénero: %4\nDescripción: %5\nCalificación: ★ %6")
                    .arg(serie.getId(), 0, 16)
                    .arg(serie.getIdSerie())
                    .arg(QString::fromStdString(serie.getNombre()))
                    .arg(QString::fromStdString(serie.getGenero()))
                    .arg(QString::fromStdString(serie.getDescripcion()))
                    .arg(QString::number(promedio, 'f', 1)));
            info->setWordWrap(true);
            infoLayout->addWidget(info);
            topLayout->addLayout(infoLayout);
            serieLayout->addLayout(topLayout);

            QTreeWidget *chaptersTree = new QTreeWidget();
            chaptersTree->setHeaderLabel("Temporadas y Capítulos");
            std::map<int, QTreeWidgetItem*> seasons;
            for (const auto& capitulo : serie.capitulos) {
                int season = capitulo.getIdTemp();
                if (seasons.find(season) == seasons.end()) {
                    seasons[season] = new QTreeWidgetItem(chaptersTree);
                    seasons[season]->setText(0, QString("Temporada %1").arg(season));
                }
                QTreeWidgetItem *chapterItem = new QTreeWidgetItem(seasons[season]);
                chapterItem->setText(0, QString("Capítulo %1 - %2").arg(capitulo.getIdCap()).arg(QString::fromStdString(capitulo.getNombre())));
                chapterItem->setData(0, Qt::UserRole, QVariant::fromValue(const_cast<Cap*>(&capitulo)));
            }
            chaptersTree->expandAll();
            serieLayout->addWidget(chaptersTree);

            stackedWidget->addWidget(serieView);

            QWidget *chapterView = new QWidget();
            QVBoxLayout *chapterLayout = new QVBoxLayout(chapterView);
            QPushButton *chapterBackButton = new QPushButton("Regresar");
            chapterLayout->addWidget(chapterBackButton);

            QHBoxLayout *chapterTopLayout = new QHBoxLayout();
            QLabel *chapterPoster = new QLabel();
            if (loaded) {
                chapterPoster->setPixmap(pixmap.scaled(300, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
            chapterTopLayout->addWidget(chapterPoster);

            QVBoxLayout *chapterInfoLayout = new QVBoxLayout();
            QLabel *chapterInfo = new QLabel();
            chapterInfo->setWordWrap(true);
            chapterInfoLayout->addWidget(chapterInfo);
            chapterTopLayout->addLayout(chapterInfoLayout);
            chapterLayout->addLayout(chapterTopLayout);

            QVideoWidget *chapterVideoWidget = new QVideoWidget();
            chapterVideoWidget->setFixedSize(600, 400);
            QMediaPlayer *chapterPlayer = new QMediaPlayer();
            chapterPlayer->setVideoOutput(chapterVideoWidget);
            chapterLayout->addWidget(chapterVideoWidget);

            // Botón de pausa/play con cambio de texto para capítulos
            QPushButton *pauseButton = new QPushButton("Pausar");
            chapterLayout->addWidget(pauseButton);
            QObject::connect(pauseButton, &QPushButton::clicked, [chapterPlayer, pauseButton]() {
                if (chapterPlayer->state() == QMediaPlayer::PlayingState) {
                    chapterPlayer->pause();
                    pauseButton->setText("Reanudar");
                } else if (chapterPlayer->state() == QMediaPlayer::PausedState) {
                    chapterPlayer->play();
                    pauseButton->setText("Pausar");
                }
            });

            QHBoxLayout *chapterRatingLayout = new QHBoxLayout();
            QLabel *chapterRatingLabel = new QLabel("Calificar:");
            QComboBox *chapterRatingCombo = new QComboBox();
            chapterRatingCombo->addItems({"1", "2", "3", "4", "5"});
            QPushButton *chapterSubmitRating = new QPushButton("Enviar");
            chapterRatingLayout->addWidget(chapterRatingLabel);
            chapterRatingLayout->addWidget(chapterRatingCombo);
            chapterRatingLayout->addWidget(chapterSubmitRating);
            chapterLayout->addLayout(chapterRatingLayout);

            stackedWidget->addWidget(chapterView);

            QObject::connect(btn, &QPushButton::clicked, [stackedWidget, serieView]() {
                stackedWidget->setCurrentWidget(serieView);
            });
            QObject::connect(backButton, &QPushButton::clicked, [stackedWidget, mainView, &mostrarSeries]() {
                stackedWidget->setCurrentWidget(mainView);
                mostrarSeries();
            });
            QObject::connect(chaptersTree, &QTreeWidget::itemDoubleClicked, [stackedWidget, chapterView, chapterInfo, chapterPlayer, &serie](QTreeWidgetItem *item, int) {
                Cap *capitulo = item->data(0, Qt::UserRole).value<Cap*>();
                if (!capitulo) return;
                QString infoCap = QString("ID: 0x%1\nID Serie: %2\nTemporada: %3\nCapítulo: %4\nNombre: %5\nDuración: %6h %7m %8s\nDescripción: %9\nCalificación: ★ %10")
                    .arg(capitulo->getId(), 0, 16)
                    .arg(capitulo->getIdSerie())
                    .arg(capitulo->getIdTemp())
                    .arg(capitulo->getIdCap())
                    .arg(QString::fromStdString(capitulo->getNombre()))
                    .arg(capitulo->getHoras())
                    .arg(capitulo->getMinutos())
                    .arg(capitulo->getSegundos())
                    .arg(QString::fromStdString(capitulo->getDescripcion()))
                    .arg(QString::number(calcularPromedio(capitulo->getCalificaciones()), 'f', 1));
                chapterInfo->setText(infoCap);

                QString videoPath = QDir::currentPath() + "/videos/" + QString::fromStdString(capitulo->getNombre()) + ".mp4";
                if (QFile::exists(videoPath)) {
                    chapterPlayer->setMedia(QUrl::fromLocalFile(QFileInfo(videoPath).absoluteFilePath()));
                    chapterPlayer->play();
                } else {
                    qDebug() << "Video no encontrado:" << videoPath;
                    chapterPlayer->setMedia(QUrl());
                }
                stackedWidget->setCurrentWidget(chapterView);
            });
            QObject::connect(chapterBackButton, &QPushButton::clicked, [stackedWidget, serieView, chapterPlayer]() {
                chapterPlayer->stop();
                stackedWidget->setCurrentWidget(serieView);
            });
            QObject::connect(chapterSubmitRating, &QPushButton::clicked, [chaptersTree, chapterRatingCombo, chapterInfo]() {
                Cap *capitulo = chaptersTree->currentItem()->data(0, Qt::UserRole).value<Cap*>();
                if (!capitulo) return;
                double rating = chapterRatingCombo->currentText().toDouble();
                capitulo->setCalificaciones(rating);
                QMessageBox::information(nullptr, "Éxito", "Calificación agregada.");
                chapterInfo->setText(
                    QString("ID: 0x%1\nID Serie: %2\nTemporada: %3\nCapítulo: %4\nNombre: %5\nDuración: %6h %7m %8s\nDescripción: %9\nCalificación: ★ %10")
                        .arg(capitulo->getId(), 0, 16)
                        .arg(capitulo->getIdSerie())
                        .arg(capitulo->getIdTemp())
                        .arg(capitulo->getIdCap())
                        .arg(QString::fromStdString(capitulo->getNombre()))
                        .arg(capitulo->getHoras())
                        .arg(capitulo->getMinutos())
                        .arg(capitulo->getSegundos())
                        .arg(QString::fromStdString(capitulo->getDescripcion()))
                        .arg(QString::number(calcularPromedio(capitulo->getCalificaciones()), 'f', 1))
                );
            });
        }
    };

    // Función para mostrar películas y series juntos con filtro
    mostrarTodoFiltrado = [&](const QString &filtro) {
        QLayoutItem *child;
        while ((child = contentLayout->takeAt(0)) != nullptr) {
            delete child->widget();
            delete child;
        }
        QString filtroSinAcentos = quitarAcentos(filtro).toLower();
        int row = 0, col = 0;

        // Mostrar películas
        for (const auto& pelicula : peliculas) {
            QString titulo = QString::fromStdString(pelicula.getNombre());
            QString descripcion = QString::fromStdString(pelicula.getDescripcion());
            QString genero = QString::fromStdString(pelicula.getGenero());
            QString tituloSinAcentos = quitarAcentos(titulo).toLower();
            QString descripcionSinAcentos = quitarAcentos(descripcion).toLower();
            QString generoSinAcentos = quitarAcentos(genero).toLower();
            if (!filtroSinAcentos.isEmpty() &&
                !tituloSinAcentos.contains(filtroSinAcentos) &&
                !descripcionSinAcentos.contains(filtroSinAcentos) &&
                !generoSinAcentos.contains(filtroSinAcentos)) {
                continue;
            }
            QPushButton *btn = new QPushButton();
            btn->setFixedSize(150, 200);
            QString baseName = titulo;
            QString imagePath = loadImagePath(baseName, resourceDir);
            QPixmap pixmap;
            bool loaded = false;
            if (!imagePath.isEmpty() && pixmap.load(imagePath)) {
                QIcon icon(pixmap.scaled(140, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                btn->setIcon(icon);
                btn->setIconSize(QSize(140, 180));
                loaded = true;
            }
            if (!loaded) {
                btn->setStyleSheet("QPushButton { text-align: bottom; padding-top: 10px; }");
            }
            double promedio = calcularPromedio(pelicula.getCalificaciones());
            btn->setText(QString("%1\n★ %2").arg(baseName).arg(QString::number(promedio, 'f', 1)));
            contentLayout->addWidget(btn, row, col);
            col++;
            if (col >= 4) {
                col = 0;
                row++;
            }
            // ...código para vista de detalle de película igual que en mostrarPeliculas...
            QWidget *movieView = new QWidget();
            QVBoxLayout *movieLayout = new QVBoxLayout(movieView);
            QPushButton *backButton = new QPushButton("Regresar");
            movieLayout->addWidget(backButton);
            QHBoxLayout *topLayout = new QHBoxLayout();
            QLabel *poster = new QLabel();
            if (loaded) {
                poster->setPixmap(pixmap.scaled(300, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                poster->setVisible(true);
                poster->repaint();
            } else {
                poster->setText("Sin imagen");
            }
            topLayout->addWidget(poster);
            QVBoxLayout *infoLayout = new QVBoxLayout();
            QLabel *info = new QLabel(
                QString("ID: 0x%1\nPelícula ID: %2\nNombre: %3\nDuración: %4h %5m %6s\nGénero: %7\nDescripción: %8\nCalificación: ★ %9")
                    .arg(pelicula.getId(), 0, 16)
                    .arg(pelicula.getIdPeli())
                    .arg(titulo)
                    .arg(pelicula.getHoras())
                    .arg(pelicula.getMinutos())
                    .arg(pelicula.getSegundos())
                    .arg(genero)
                    .arg(descripcion)
                    .arg(QString::number(promedio, 'f', 1)));
            info->setWordWrap(true);
            infoLayout->addWidget(info);
            topLayout->addLayout(infoLayout);
            movieLayout->addLayout(topLayout);
            QVideoWidget *videoWidget = new QVideoWidget();
            videoWidget->setFixedSize(600, 400);
            QMediaPlayer *player = new QMediaPlayer();
            player->setVideoOutput(videoWidget);
            QString videoPath = QDir::currentPath() + "/videos/" + baseName + ".mp4";
            if (QFile::exists(videoPath)) {
                player->setMedia(QUrl::fromLocalFile(QFileInfo(videoPath).absoluteFilePath()));
                player->play();
            } else {
                qDebug() << "Video no encontrado:" << videoPath;
            }
            movieLayout->addWidget(videoWidget);
            QPushButton *pauseButton = new QPushButton("Pausar");
            movieLayout->addWidget(pauseButton);
            QObject::connect(pauseButton, &QPushButton::clicked, [player, pauseButton]() {
                if (player->state() == QMediaPlayer::PlayingState) {
                    player->pause();
                    pauseButton->setText("Reanudar");
                } else if (player->state() == QMediaPlayer::PausedState) {
                    player->play();
                    pauseButton->setText("Pausar");
                }
            });
            QHBoxLayout *ratingLayout = new QHBoxLayout();
            QComboBox *ratingCombo = new QComboBox();
            ratingCombo->addItems({"1", "2", "3", "4", "5"});
            QPushButton *submitRating = new QPushButton("Enviar");
            ratingLayout->addWidget(new QLabel("Calificar:"));
            ratingLayout->addWidget(ratingCombo);
            ratingLayout->addWidget(submitRating);
            movieLayout->addLayout(ratingLayout);
            stackedWidget->addWidget(movieView);
            QObject::connect(btn, &QPushButton::clicked, [stackedWidget, movieView]() {
                stackedWidget->setCurrentWidget(movieView);
            });
            QObject::connect(backButton, &QPushButton::clicked, [stackedWidget, mainView, player, filtro, &mostrarTodoFiltrado]() mutable {
                player->stop();
                stackedWidget->setCurrentWidget(mainView);
                mostrarTodoFiltrado(filtro);
            });
            QObject::connect(submitRating, &QPushButton::clicked, [ratingCombo, &pelicula, info, &peliculas, &series, &window]() {
                double rating = ratingCombo->currentText().toDouble();
                const_cast<Pelis&>(pelicula).setCalificaciones(rating);
                guardarJSON(peliculas, series, &window);
                QMessageBox::information(nullptr, "Éxito", "Calificación agregada.");
                double nuevoPromedio = calcularPromedio(pelicula.getCalificaciones());
                info->setText(info->text().replace(QRegularExpression("Calificación: ★ [0-9.]+"),
                                                   QString("Calificación: ★ %1").arg(QString::number(nuevoPromedio, 'f', 1))));
            });
        }

        // Mostrar series
        for (const auto& serie : series) {
            QString titulo = QString::fromStdString(serie.getNombre());
            QString descripcion = QString::fromStdString(serie.getDescripcion());
            QString genero = QString::fromStdString(serie.getGenero());
            QString tituloSinAcentos = quitarAcentos(titulo).toLower();
            QString descripcionSinAcentos = quitarAcentos(descripcion).toLower();
            QString generoSinAcentos = quitarAcentos(genero).toLower();
            if (!filtroSinAcentos.isEmpty() &&
                !tituloSinAcentos.contains(filtroSinAcentos) &&
                !descripcionSinAcentos.contains(filtroSinAcentos) &&
                !generoSinAcentos.contains(filtroSinAcentos)) {
                continue;
            }
            QPushButton *btn = new QPushButton();
            btn->setFixedSize(150, 200);
            QString baseName = titulo;
            QString imagePath = loadImagePath(baseName, resourceDir);
            QPixmap pixmap;
            bool loaded = false;
            if (!imagePath.isEmpty() && pixmap.load(imagePath)) {
                QIcon icon(pixmap.scaled(140, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                btn->setIcon(icon);
                btn->setIconSize(QSize(140, 180));
                loaded = true;
            }
            if (!loaded) {
                btn->setStyleSheet("QPushButton { text-align: bottom; padding-top: 10px; }");
            }
            double promedio = calcularPromedioSerie(serie);
            btn->setText(QString("%1\n★ %2").arg(baseName).arg(QString::number(promedio, 'f', 1)));
            contentLayout->addWidget(btn, row, col);
            col++;
            if (col >= 4) {
                col = 0;
                row++;
            }
            // ...código para vista de detalle de serie igual que en mostrarSeries...
            QWidget *serieView = new QWidget();
            QVBoxLayout *serieLayout = new QVBoxLayout(serieView);
            QPushButton *backButton = new QPushButton("Regresar");
            serieLayout->addWidget(backButton);

            QHBoxLayout *topLayout = new QHBoxLayout();
            QLabel *poster = new QLabel();
            if (loaded) {
                poster->setPixmap(pixmap.scaled(300, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
            topLayout->addWidget(poster);

            QVBoxLayout *infoLayout = new QVBoxLayout();
            QLabel *info = new QLabel(
                QString("ID: 0x%1\nID Serie: %2\nNombre: %3\nGénero: %4\nDescripción: %5\nCalificación: ★ %6")
                    .arg(serie.getId(), 0, 16)
                    .arg(serie.getIdSerie())
                    .arg(QString::fromStdString(serie.getNombre()))
                    .arg(QString::fromStdString(serie.getGenero()))
                    .arg(QString::fromStdString(serie.getDescripcion()))
                    .arg(QString::number(promedio, 'f', 1)));
            info->setWordWrap(true);
            infoLayout->addWidget(info);
            topLayout->addLayout(infoLayout);
            serieLayout->addLayout(topLayout);

            QTreeWidget *chaptersTree = new QTreeWidget();
            chaptersTree->setHeaderLabel("Temporadas y Capítulos");
            std::map<int, QTreeWidgetItem*> seasons;
            for (const auto& capitulo : serie.capitulos) {
                int season = capitulo.getIdTemp();
                if (seasons.find(season) == seasons.end()) {
                    seasons[season] = new QTreeWidgetItem(chaptersTree);
                    seasons[season]->setText(0, QString("Temporada %1").arg(season));
                }
                QTreeWidgetItem *chapterItem = new QTreeWidgetItem(seasons[season]);
                chapterItem->setText(0, QString("Capítulo %1 - %2").arg(capitulo.getIdCap()).arg(QString::fromStdString(capitulo.getNombre())));
                chapterItem->setData(0, Qt::UserRole, QVariant::fromValue(const_cast<Cap*>(&capitulo)));
            }
            chaptersTree->expandAll();
            serieLayout->addWidget(chaptersTree);

            stackedWidget->addWidget(serieView);

            QWidget *chapterView = new QWidget();
            QVBoxLayout *chapterLayout = new QVBoxLayout(chapterView);
            QPushButton *chapterBackButton = new QPushButton("Regresar");
            chapterLayout->addWidget(chapterBackButton);

            QHBoxLayout *chapterTopLayout = new QHBoxLayout();
            QLabel *chapterPoster = new QLabel();
            if (loaded) {
                chapterPoster->setPixmap(pixmap.scaled(300, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
            chapterTopLayout->addWidget(chapterPoster);

            QVBoxLayout *chapterInfoLayout = new QVBoxLayout();
            QLabel *chapterInfo = new QLabel();
            chapterInfo->setWordWrap(true);
            chapterInfoLayout->addWidget(chapterInfo);
            chapterTopLayout->addLayout(chapterInfoLayout);
            chapterLayout->addLayout(chapterTopLayout);

            QVideoWidget *chapterVideoWidget = new QVideoWidget();
            chapterVideoWidget->setFixedSize(600, 400);
            QMediaPlayer *chapterPlayer = new QMediaPlayer();
            chapterPlayer->setVideoOutput(chapterVideoWidget);
            chapterLayout->addWidget(chapterVideoWidget);

            // Botón de pausa/play con cambio de texto para capítulos
            QPushButton *pauseButton = new QPushButton("Pausar");
            chapterLayout->addWidget(pauseButton);
            QObject::connect(pauseButton, &QPushButton::clicked, [chapterPlayer, pauseButton]() {
                if (chapterPlayer->state() == QMediaPlayer::PlayingState) {
                    chapterPlayer->pause();
                    pauseButton->setText("Reanudar");
                } else if (chapterPlayer->state() == QMediaPlayer::PausedState) {
                    chapterPlayer->play();
                    pauseButton->setText("Pausar");
                }
            });

            QHBoxLayout *chapterRatingLayout = new QHBoxLayout();
            QLabel *chapterRatingLabel = new QLabel("Calificar:");
            QComboBox *chapterRatingCombo = new QComboBox();
            chapterRatingCombo->addItems({"1", "2", "3", "4", "5"});
            QPushButton *chapterSubmitRating = new QPushButton("Enviar");
            chapterRatingLayout->addWidget(chapterRatingLabel);
            chapterRatingLayout->addWidget(chapterRatingCombo);
            chapterRatingLayout->addWidget(chapterSubmitRating);
            chapterLayout->addLayout(chapterRatingLayout);

            stackedWidget->addWidget(chapterView);

            QObject::connect(btn, &QPushButton::clicked, [stackedWidget, serieView]() {
                stackedWidget->setCurrentWidget(serieView);
            });
            QObject::connect(backButton, &QPushButton::clicked, [stackedWidget, mainView, &mostrarSeries]() {
                stackedWidget->setCurrentWidget(mainView);
                mostrarSeries();
            });
            QObject::connect(chaptersTree, &QTreeWidget::itemDoubleClicked, [stackedWidget, chapterView, chapterInfo, chapterPlayer, &serie](QTreeWidgetItem *item, int) {
                Cap *capitulo = item->data(0, Qt::UserRole).value<Cap*>();
                if (!capitulo) return;
                QString infoCap = QString("ID: 0x%1\nID Serie: %2\nTemporada: %3\nCapítulo: %4\nNombre: %5\nDuración: %6h %7m %8s\nDescripción: %9\nCalificación: ★ %10")
                    .arg(capitulo->getId(), 0, 16)
                    .arg(capitulo->getIdSerie())
                    .arg(capitulo->getIdTemp())
                    .arg(capitulo->getIdCap())
                    .arg(QString::fromStdString(capitulo->getNombre()))
                    .arg(capitulo->getHoras())
                    .arg(capitulo->getMinutos())
                    .arg(capitulo->getSegundos())
                    .arg(QString::fromStdString(capitulo->getDescripcion()))
                    .arg(QString::number(calcularPromedio(capitulo->getCalificaciones()), 'f', 1));
                chapterInfo->setText(infoCap);

                QString videoPath = QDir::currentPath() + "/videos/" + QString::fromStdString(capitulo->getNombre()) + ".mp4";
                if (QFile::exists(videoPath)) {
                    chapterPlayer->setMedia(QUrl::fromLocalFile(QFileInfo(videoPath).absoluteFilePath()));
                    chapterPlayer->play();
                } else {
                    qDebug() << "Video no encontrado:" << videoPath;
                    chapterPlayer->setMedia(QUrl());
                }
                stackedWidget->setCurrentWidget(chapterView);
            });
            QObject::connect(chapterBackButton, &QPushButton::clicked, [stackedWidget, serieView, chapterPlayer]() {
                chapterPlayer->stop();
                stackedWidget->setCurrentWidget(serieView);
            });
            QObject::connect(chapterSubmitRating, &QPushButton::clicked, [chaptersTree, chapterRatingCombo, chapterInfo]() {
                Cap *capitulo = chaptersTree->currentItem()->data(0, Qt::UserRole).value<Cap*>();
                if (!capitulo) return;
                double rating = chapterRatingCombo->currentText().toDouble();
                capitulo->setCalificaciones(rating);
                QMessageBox::information(nullptr, "Éxito", "Calificación agregada.");
                chapterInfo->setText(
                    QString("ID: 0x%1\nID Serie: %2\nTemporada: %3\nCapítulo: %4\nNombre: %5\nDuración: %6h %7m %8s\nDescripción: %9\nCalificación: ★ %10")
                        .arg(capitulo->getId(), 0, 16)
                        .arg(capitulo->getIdSerie())
                        .arg(capitulo->getIdTemp())
                        .arg(capitulo->getIdCap())
                        .arg(QString::fromStdString(capitulo->getNombre()))
                        .arg(capitulo->getHoras())
                        .arg(capitulo->getMinutos())
                        .arg(capitulo->getSegundos())
                        .arg(QString::fromStdString(capitulo->getDescripcion()))
                        .arg(QString::number(calcularPromedio(capitulo->getCalificaciones()), 'f', 1))
                );
            });
        }
    };

    // --- Estado y lógica para el modo de búsqueda ---
    enum class SearchMode { Peliculas, Series, Todo };
    SearchMode currentSearchMode = SearchMode::Peliculas;

    // Redefinir mostrarPeliculas y mostrarSeries para usar el filtro actual
    mostrarPeliculas = [&]() {
        currentSearchMode = SearchMode::Peliculas;
        mostrarPeliculasFiltrado(lastSearch);
    };
    mostrarSeries = [&]() {
        currentSearchMode = SearchMode::Series;
        mostrarSeriesFiltrado(lastSearch);
    };
    auto mostrarTodo = [&]() {
        currentSearchMode = SearchMode::Todo;
        mostrarTodoFiltrado(lastSearch);
    };

    // --- Conectar búsqueda para películas, series o todo según el modo actual ---
    QObject::connect(searchButton, &QPushButton::clicked, [&]() {
        lastSearch = searchInput->text();
        if (currentSearchMode == SearchMode::Peliculas) {
            mostrarPeliculasFiltrado(lastSearch);
            stackedWidget->setCurrentWidget(mainView);
        } else if (currentSearchMode == SearchMode::Series) {
            mostrarSeriesFiltrado(lastSearch);
            stackedWidget->setCurrentWidget(mainView);
        } else {
            mostrarTodoFiltrado(lastSearch);
            stackedWidget->setCurrentWidget(mainView);
        }
    });
    QObject::connect(searchInput, &QLineEdit::returnPressed, [&]() {
        lastSearch = searchInput->text();
        if (currentSearchMode == SearchMode::Peliculas) {
            mostrarPeliculasFiltrado(lastSearch);
            stackedWidget->setCurrentWidget(mainView);
        } else if (currentSearchMode == SearchMode::Series) {
            mostrarSeriesFiltrado(lastSearch);
            stackedWidget->setCurrentWidget(mainView);
        } else {
            mostrarTodoFiltrado(lastSearch);
            stackedWidget->setCurrentWidget(mainView);
        }
    });

    // Cambiar el modo de búsqueda al navegar
    QObject::connect(btnPeliculas, &QPushButton::clicked, [&]() {
        currentSearchMode = SearchMode::Peliculas;
        mostrarPeliculas();
    });
    QObject::connect(btnSeries, &QPushButton::clicked, [&]() {
        currentSearchMode = SearchMode::Series;
        mostrarSeries();
    });
    QObject::connect(btnTodo, &QPushButton::clicked, [&]() {
        currentSearchMode = SearchMode::Todo;
        mostrarTodo();
    });

    // Mostrar películas por defecto
    mostrarPeliculas();

    // Configurar ventana
    window.setCentralWidget(stackedWidget);

    // Guardar al cerrar
    QObject::connect(&app, &QApplication::aboutToQuit, [&]() {
        guardarJSON(peliculas, series, &window);
    });

    window.show();
    return app.exec();
}
