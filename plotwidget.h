#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPainter>
#include <QColor>
#include <QDateTime>

struct PlotData {
    QVector<double> values;
    QColor color;
    QString name;
    bool visible;
    
    PlotData() : visible(true) {}
};

class PlotWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlotWidget(QWidget *parent = nullptr);
    
    void addDataPoint(int channel, double value);
    void clearData();
    void setChannelVisible(int channel, bool visible);
    void setChannelColor(int channel, const QColor &color);
    void setMaxDataPoints(int max);
    void setPlotTexts(const QString &title, const QString &yLabel, 
                     const QString &xLabel, const QString &waitingText);
    
protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    QVector<PlotData> channels;
    int maxDataPoints;
    double minValue;
    double maxValue;
    bool autoScale;
    int maxPoints;  // Track max points for X-axis labels
    
    // Translatable texts
    QString plotTitle;
    QString yAxisLabel;
    QString xAxisLabel;
    QString waitingMessage;
    
    void updateMinMax();
    QPointF dataToScreen(int index, double value, int totalPoints);
};

#endif // PLOTWIDGET_H
