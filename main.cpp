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
    QStringList extensions = {".webp"};
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

QString quitarAcentos(const QString& texto) {
    QString resultado;
    for (QChar c : texto) {
        ushort u = c.unicode();
        switch (u) {
            case 0x00E1: case 0x00C1: resultado += 'a'; break;
            case 0x00E9: case 0x00C9: resultado += 'e'; break;
            case 0x00ED: case 0x00CD: resultado += 'i'; break;
            case 0x00F3: case 0x00D3: resultado += 'o'; break;
            case 0x00FA: case 0x00DA: resultado += 'u'; break;
            case 0x00FC: case 0x00DC: resultado += 'u'; break;
            case 0x00F1: case 0x00D1: resultado += 'n'; break;
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

    QString resourceDir = QCoreApplication::applicationDirPath();
    qDebug() << "Directorio de recursos:" << resourceDir;

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

    QStackedWidget *stackedWidget = new QStackedWidget(&window);

    QWidget *mainView = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(mainView);

    QHBoxLayout *searchLayout = new QHBoxLayout();
    QLineEdit *searchInput = new QLineEdit();
    searchInput->setPlaceholderText("Buscar por título, descripción o género...");
    QPushButton *searchButton = new QPushButton("Buscar");
    searchLayout->addWidget(searchInput);
    searchLayout->addWidget(searchButton);
    mainLayout->insertLayout(0, searchLayout);

    QString lastSearch = "";

    QPushButton *backButtonMain = new QPushButton("Regresar", mainView);
    backButtonMain->setVisible(false);
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

    std::function<void()> mostrarPeliculas;
    std::function<void()> mostrarSeries;
    std::function<void(const QString &)> mostrarPeliculasFiltrado;
    std::function<void(const QString &)> mostrarSeriesFiltrado;
    std::function<void(const QString &)> mostrarTodoFiltrado;

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
            QWidget *itemWidget = new QWidget();
            QVBoxLayout *itemLayout = new QVBoxLayout(itemWidget);
            itemLayout->setContentsMargins(0, 0, 0, 0);
            itemLayout->setSpacing(5);

            QString baseName = titulo;
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
            itemLayout->addWidget(imageLabel, 0, Qt::AlignHCenter);

            double promedio = calcularPromedio(pelicula.getCalificaciones());
            QPushButton *actionButton = new QPushButton(
                QString("%1\n★ %2").arg(baseName).arg(QString::number(promedio, 'f', 1))
            );
            actionButton->setFixedWidth(140);
            itemLayout->addWidget(actionButton, 0, Qt::AlignHCenter);

            itemWidget->setLayout(itemLayout);
            contentLayout->addWidget(itemWidget, row, col);
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
                const_cast<Pelis&>(pelicula) += rating;
                guardarJSON(peliculas, series, &window);
                QMessageBox::information(nullptr, "Éxito", "Calificación agregada.");
                double nuevoPromedio = calcularPromedio(pelicula.getCalificaciones());
                info->setText(info->text().replace(QRegularExpression("Calificación: ★ [0-9.]+"),
                                                   QString("Calificación: ★ %1").arg(QString::number(nuevoPromedio, 'f', 1))));
            });
        }
    };

    mostrarSeriesFiltrado = [&](const QString &filtro) {
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
            // --- Widget vertical con imagen y botón (igual que películas) ---
            QWidget *itemWidget = new QWidget();
            QVBoxLayout *itemLayout = new QVBoxLayout(itemWidget);
            itemLayout->setContentsMargins(0, 0, 0, 0);
            itemLayout->setSpacing(5);

            // Imagen arriba (QLabel)
            QString baseName = titulo;
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
            itemLayout->addWidget(imageLabel, 0, Qt::AlignHCenter);

            // Botón abajo con título y calificación
            double promedio = calcularPromedioSerie(serie);
            QPushButton *actionButton = new QPushButton(
                QString("%1\n★ %2").arg(baseName).arg(QString::number(promedio, 'f', 1))
            );
            actionButton->setFixedWidth(140);
            itemLayout->addWidget(actionButton, 0, Qt::AlignHCenter);

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

            // --- NUEVO: Vista de capítulo y conexión para reproducir video ---
            QObject::connect(chaptersTree, &QTreeWidget::itemDoubleClicked, [&, stackedWidget, serieView, mainView, serie, &series, &peliculas, &window, resourceDir](QTreeWidgetItem *item, int) {
                QVariant var = item->data(0, Qt::UserRole);
                if (!var.isValid()) return;
                Cap* capitulo = var.value<Cap*>();
                if (!capitulo) return;

                QWidget *chapterView = new QWidget();
                QVBoxLayout *chapterLayout = new QVBoxLayout(chapterView);
                QPushButton *backButton = new QPushButton("Regresar");
                chapterLayout->addWidget(backButton);

                QHBoxLayout *topLayout = new QHBoxLayout();
                QLabel *poster = new QLabel();
                QString baseName = QString::fromStdString(serie.getNombre());
                QString imagePath = loadImagePath(baseName, resourceDir);
                QPixmap pixmap;
                if (!imagePath.isEmpty() && pixmap.load(imagePath)) {
                    poster->setPixmap(pixmap.scaled(200, 260, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    poster->setAlignment(Qt::AlignCenter);
                } else {
                    poster->setText("Sin imagen");
                    poster->setAlignment(Qt::AlignCenter);
                }
                topLayout->addWidget(poster);

                QVBoxLayout *infoLayout = new QVBoxLayout();
                double promedio = calcularPromedio(capitulo->getCalificaciones());
                QLabel *info = new QLabel(
                    QString("ID: 0x%1\nTemporada: %2\nCapítulo: %3\nNombre: %4\nDuración: %5h %6m %7s\nDescripción: %8\nCalificación: ★ %9")
                        .arg(capitulo->getId(), 0, 16)
                        .arg(capitulo->getIdTemp())
                        .arg(capitulo->getIdCap())
                        .arg(QString::fromStdString(capitulo->getNombre()))
                        .arg(capitulo->getHoras())
                        .arg(capitulo->getMinutos())
                        .arg(capitulo->getSegundos())
                        .arg(QString::fromStdString(capitulo->getDescripcion()))
                        .arg(QString::number(promedio, 'f', 1)));
                info->setWordWrap(true);
                infoLayout->addWidget(info);
                topLayout->addLayout(infoLayout);
                chapterLayout->addLayout(topLayout);

                QVideoWidget *videoWidget = new QVideoWidget();
                videoWidget->setFixedSize(600, 400);
                QMediaPlayer *player = new QMediaPlayer(chapterView);
                player->setVideoOutput(videoWidget);
                // Ruta: /videos/{Serie}/{Capitulo}.mp4
                QString videoPath = resourceDir + "/videos/" + QString::fromStdString(capitulo->getNombre()) + ".mp4";
                if (QFile::exists(videoPath)) {
                    player->setMedia(QUrl::fromLocalFile(QFileInfo(videoPath).absoluteFilePath()));
                } else {
                    qDebug() << "Video de capítulo no encontrado:" << videoPath;
                }
                chapterLayout->addWidget(videoWidget);

                QPushButton *pauseButton = new QPushButton("Pausar");
                chapterLayout->addWidget(pauseButton);
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
                chapterLayout->addLayout(ratingLayout);

                stackedWidget->addWidget(chapterView);
                stackedWidget->setCurrentWidget(chapterView);

                if (QFile::exists(videoPath)) {
                    player->setMedia(QUrl::fromLocalFile(QFileInfo(videoPath).absoluteFilePath()));
                    player->play();
                }

                QObject::connect(backButton, &QPushButton::clicked, [stackedWidget, serieView, player, chapterView]() {
                    player->stop();
                    stackedWidget->setCurrentWidget(serieView);
                    stackedWidget->removeWidget(chapterView);
                    chapterView->deleteLater();
                });
                QObject::connect(submitRating, &QPushButton::clicked, [ratingCombo, capitulo, info, &peliculas, &series, &window]() {
                    double rating = ratingCombo->currentText().toDouble();
                    capitulo->setCalificaciones(rating);
                    guardarJSON(peliculas, series, &window);
                    QMessageBox::information(nullptr, "Éxito", "Calificación agregada.");
                    double nuevoPromedio = calcularPromedio(capitulo->getCalificaciones());
                    info->setText(info->text().replace(QRegularExpression("Calificación: ★ [0-9.]+"),
                                                       QString("Calificación: ★ %1").arg(QString::number(nuevoPromedio, 'f', 1))));
                });
            });
            // --- FIN NUEVO ---

            stackedWidget->addWidget(serieView);

            QObject::connect(actionButton, &QPushButton::clicked, [stackedWidget, serieView]() {
                stackedWidget->setCurrentWidget(serieView);
            });
            QObject::connect(backButton, &QPushButton::clicked, [stackedWidget, mainView, &mostrarSeriesFiltrado, filtro]() {
                stackedWidget->setCurrentWidget(mainView);
                mostrarSeriesFiltrado(filtro);
            });
        }
    };

    mostrarTodoFiltrado = [&](const QString &filtro) {
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
            QWidget *itemWidget = new QWidget();
            QVBoxLayout *itemLayout = new QVBoxLayout(itemWidget);
            itemLayout->setContentsMargins(0, 0, 0, 0);
            itemLayout->setSpacing(5);

            QString baseName = titulo;
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
            itemLayout->addWidget(imageLabel, 0, Qt::AlignHCenter);

            double promedio = calcularPromedio(pelicula.getCalificaciones());
            QPushButton *actionButton = new QPushButton(
                QString("%1\n★ %2").arg(baseName).arg(QString::number(promedio, 'f', 1))
            );
            actionButton->setFixedWidth(140);
            itemLayout->addWidget(actionButton, 0, Qt::AlignHCenter);

            itemWidget->setLayout(itemLayout);
            contentLayout->addWidget(itemWidget, row, col);
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
        }

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
            QWidget *itemWidget = new QWidget();
            QVBoxLayout *itemLayout = new QVBoxLayout(itemWidget);
            itemLayout->setContentsMargins(0, 0, 0, 0);
            itemLayout->setSpacing(5);

            QString baseName = titulo;
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
            itemLayout->addWidget(imageLabel, 0, Qt::AlignHCenter);

            double promedio = calcularPromedioSerie(serie);
            QPushButton *actionButton = new QPushButton(
                QString("%1\n★ %2").arg(baseName).arg(QString::number(promedio, 'f', 1))
            );
            actionButton->setFixedWidth(140);
            itemLayout->addWidget(actionButton, 0, Qt::AlignHCenter);

            itemWidget->setLayout(itemLayout);
            contentLayout->addWidget(itemWidget, row, col);
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

            // --- NUEVO: Vista de capítulo y conexión para reproducir video ---
            QObject::connect(chaptersTree, &QTreeWidget::itemDoubleClicked, [&, stackedWidget, serieView, mainView, serie, &series, &peliculas, &window, resourceDir](QTreeWidgetItem *item, int) {
                QVariant var = item->data(0, Qt::UserRole);
                if (!var.isValid()) return;
                Cap* capitulo = var.value<Cap*>();
                if (!capitulo) return;

                QWidget *chapterView = new QWidget();
                QVBoxLayout *chapterLayout = new QVBoxLayout(chapterView);
                QPushButton *backButton = new QPushButton("Regresar");
                chapterLayout->addWidget(backButton);

                QHBoxLayout *topLayout = new QHBoxLayout();
                QLabel *poster = new QLabel();
                QString baseName = QString::fromStdString(serie.getNombre());
                QString imagePath = loadImagePath(baseName, resourceDir);
                QPixmap pixmap;
                if (!imagePath.isEmpty() && pixmap.load(imagePath)) {
                    poster->setPixmap(pixmap.scaled(200, 260, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    poster->setAlignment(Qt::AlignCenter);
                } else {
                    poster->setText("Sin imagen");
                    poster->setAlignment(Qt::AlignCenter);
                }
                topLayout->addWidget(poster);

                QVBoxLayout *infoLayout = new QVBoxLayout();
                double promedio = calcularPromedio(capitulo->getCalificaciones());
                QLabel *info = new QLabel(
                    QString("ID: 0x%1\nTemporada: %2\nCapítulo: %3\nNombre: %4\nDuración: %5h %6m %7s\nDescripción: %8\nCalificación: ★ %9")
                        .arg(capitulo->getId(), 0, 16)
                        .arg(capitulo->getIdTemp())
                        .arg(capitulo->getIdCap())
                        .arg(QString::fromStdString(capitulo->getNombre()))
                        .arg(capitulo->getHoras())
                        .arg(capitulo->getMinutos())
                        .arg(capitulo->getSegundos())
                        .arg(QString::fromStdString(capitulo->getDescripcion()))
                        .arg(QString::number(promedio, 'f', 1)));
                info->setWordWrap(true);
                infoLayout->addWidget(info);
                topLayout->addLayout(infoLayout);
                chapterLayout->addLayout(topLayout);

                QVideoWidget *videoWidget = new QVideoWidget();
                videoWidget->setFixedSize(600, 400);
                QMediaPlayer *player = new QMediaPlayer(chapterView);
                player->setVideoOutput(videoWidget);
                // Ruta: /videos/{Serie}/{Capitulo}.mp4
                QString videoPath = resourceDir + "/videos/" + QString::fromStdString(capitulo->getNombre()) + ".mp4";
                if (QFile::exists(videoPath)) {
                    player->setMedia(QUrl::fromLocalFile(QFileInfo(videoPath).absoluteFilePath()));
                } else {
                    qDebug() << "Video de capítulo no encontrado:" << videoPath;
                }
                chapterLayout->addWidget(videoWidget);

                QPushButton *pauseButton = new QPushButton("Pausar");
                chapterLayout->addWidget(pauseButton);
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
                chapterLayout->addLayout(ratingLayout);

                stackedWidget->addWidget(chapterView);
                stackedWidget->setCurrentWidget(chapterView);

                if (QFile::exists(videoPath)) {
                    player->setMedia(QUrl::fromLocalFile(QFileInfo(videoPath).absoluteFilePath()));
                    player->play();
                }

                QObject::connect(backButton, &QPushButton::clicked, [stackedWidget, serieView, player, chapterView]() {
                    player->stop();
                    stackedWidget->setCurrentWidget(serieView);
                    stackedWidget->removeWidget(chapterView);
                    chapterView->deleteLater();
                });
                QObject::connect(submitRating, &QPushButton::clicked, [ratingCombo, capitulo, info, &peliculas, &series, &window]() {
                    double rating = ratingCombo->currentText().toDouble();
                    capitulo->setCalificaciones(rating);
                    guardarJSON(peliculas, series, &window);
                    QMessageBox::information(nullptr, "Éxito", "Calificación agregada.");
                    double nuevoPromedio = calcularPromedio(capitulo->getCalificaciones());
                    info->setText(info->text().replace(QRegularExpression("Calificación: ★ [0-9.]+"),
                                                       QString("Calificación: ★ %1").arg(QString::number(nuevoPromedio, 'f', 1))));
                });
            });
            // --- FIN NUEVO ---

            stackedWidget->addWidget(serieView);

            QObject::connect(actionButton, &QPushButton::clicked, [stackedWidget, serieView]() {
                stackedWidget->setCurrentWidget(serieView);
            });
            QObject::connect(backButton, &QPushButton::clicked, [stackedWidget, mainView, &mostrarTodoFiltrado, filtro]() {
                stackedWidget->setCurrentWidget(mainView);
                mostrarTodoFiltrado(filtro);
            });
        }
    };

    // --- Estado y lógica para el modo de búsqueda ---
    enum class SearchMode { Peliculas, Series, Todo };
    SearchMode currentSearchMode = SearchMode::Peliculas;

    mostrarPeliculas = [&]() {
        qDebug() << "Ejecutando mostrarPeliculas";
        currentSearchMode = SearchMode::Peliculas;
        mostrarPeliculasFiltrado("");
    };

    mostrarSeries = [&]() {
        qDebug() << "Ejecutando mostrarSeries";
        currentSearchMode = SearchMode::Series;
        mostrarSeriesFiltrado("");
    };

    auto mostrarTodo = [&]() {
        qDebug() << "Ejecutando mostrarTodo";
        currentSearchMode = SearchMode::Todo;
        mostrarTodoFiltrado("");
    };

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

    QObject::connect(btnPeliculas, &QPushButton::clicked, [&]() {
        qDebug() << "Botón Películas clicado";
        mostrarPeliculas();
    });
    QObject::connect(btnSeries, &QPushButton::clicked, [&]() {
        qDebug() << "Botón Series clicado";
        mostrarSeries();
    });
    QObject::connect(btnTodo, &QPushButton::clicked, [&]() {
        qDebug() << "Botón Todo clicado";
        mostrarTodo();
    });

    mostrarPeliculas();

    window.setCentralWidget(stackedWidget);

    QObject::connect(&app, &QApplication::aboutToQuit, [&]() {
        guardarJSON(peliculas, series, &window);
    });

    window.show();
    return app.exec();
}