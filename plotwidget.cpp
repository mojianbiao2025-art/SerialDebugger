#include "plotwidget.h"
#include <QPaintEvent>
#include <QFontMetrics>
#include <cmath>

PlotWidget::PlotWidget(QWidget *parent)
    : QWidget(parent)
    , maxDataPoints(1000)
    , minValue(-2.0)
    , maxValue(2.0)
    , autoScale(true)
    , maxPoints(0)
    , plotTitle("Real-time Data Plot")
    , yAxisLabel("Value")
    , xAxisLabel("Number of points")
    , waitingMessage("Waiting for data...\nSend numeric values to plot")
{
    setMinimumSize(400, 300);
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    
    // Initialize 6 channels with different colors
    QVector<QColor> colors;
    colors << QColor(255, 0, 0)      // Red
           << QColor(0, 0, 255)      // Blue
           << QColor(0, 128, 0)      // Green
           << QColor(255, 165, 0)    // Orange
           << QColor(128, 0, 128)    // Purple
           << QColor(0, 128, 128);   // Teal
    
    for (int i = 0; i < 6; ++i) {
        PlotData data;
        data.color = colors[i];
        data.name = QString("Graph %1").arg(i + 1);
        data.visible = true;
        channels.append(data);
    }
}

void PlotWidget::addDataPoint(int channel, double value)
{
    if (channel < 0 || channel >= channels.size()) {
        return;
    }
    
    channels[channel].values.append(value);
    
    // Limit data points
    if (channels[channel].values.size() > maxDataPoints) {
        channels[channel].values.removeFirst();
    }
    
    if (autoScale) {
        updateMinMax();
    }
    
    update();
}

void PlotWidget::clearData()
{
    for (int i = 0; i < channels.size(); ++i) {
        channels[i].values.clear();
    }
    minValue = -2.0;
    maxValue = 2.0;
    update();
}

void PlotWidget::setChannelVisible(int channel, bool visible)
{
    if (channel >= 0 && channel < channels.size()) {
        channels[channel].visible = visible;
        update();
    }
}

void PlotWidget::setChannelColor(int channel, const QColor &color)
{
    if (channel >= 0 && channel < channels.size()) {
        channels[channel].color = color;
        update();
    }
}

void PlotWidget::setMaxDataPoints(int max)
{
    maxDataPoints = max;
}

void PlotWidget::updateMinMax()
{
    bool first = true;
    
    for (int i = 0; i < channels.size(); ++i) {
        if (!channels[i].visible || channels[i].values.isEmpty()) {
            continue;
        }
        
        for (double value : channels[i].values) {
            if (first) {
                minValue = maxValue = value;
                first = false;
            } else {
                if (value < minValue) minValue = value;
                if (value > maxValue) maxValue = value;
            }
        }
    }
    
    // Add some margin
    if (!first) {
        double range = maxValue - minValue;
        if (range < 0.1) range = 0.1;
        minValue -= range * 0.1;
        maxValue += range * 0.1;
    }
}

QPointF PlotWidget::dataToScreen(int index, double value, int totalPoints)
{
    const int leftMargin = 75;
    const int rightMargin = 20;
    const int topMargin = 40;
    const int bottomMargin = 50;
    
    int plotWidth = width() - leftMargin - rightMargin;
    int plotHeight = height() - topMargin - bottomMargin;
    
    double x = leftMargin + (double)index / (totalPoints - 1) * plotWidth;
    double y = topMargin + plotHeight - ((value - minValue) / (maxValue - minValue)) * plotHeight;
    
    return QPointF(x, y);
}

void PlotWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    const int leftMargin = 60;
    const int rightMargin = 20;
    const int topMargin = 30;
    const int bottomMargin = 40;
    
    int plotWidth = width() - leftMargin - rightMargin;
    int plotHeight = height() - topMargin - bottomMargin;
    
    // Draw background with gradient
    QLinearGradient bgGradient(0, 0, 0, height());
    bgGradient.setColorAt(0, QColor(250, 250, 250));
    bgGradient.setColorAt(1, QColor(240, 240, 240));
    painter.fillRect(rect(), bgGradient);
    
    // Draw plot area with white background
    painter.fillRect(leftMargin, topMargin, plotWidth, plotHeight, Qt::white);
    
    // Draw plot area border with shadow effect
    painter.setPen(QPen(QColor(52, 152, 219), 2));
    painter.drawRect(leftMargin, topMargin, plotWidth, plotHeight);
    
    // Draw grid
    painter.setPen(QPen(QColor(220, 220, 220), 1, Qt::DotLine));
    
    // Horizontal grid lines
    for (int i = 0; i <= 4; ++i) {
        int y = topMargin + i * plotHeight / 4;
        painter.drawLine(leftMargin, y, leftMargin + plotWidth, y);
        
        // Y-axis labels with better formatting
        double value = maxValue - (maxValue - minValue) * i / 4;
        QString label = QString::number(value, 'f', 2);
        
        painter.setPen(QColor(44, 62, 80));
        painter.setFont(QFont("Microsoft YaHei UI", 8));
        QRect labelRect(8, y - 10, 62, 20);
        painter.drawText(labelRect, Qt::AlignRight | Qt::AlignVCenter, label);
        painter.setPen(QPen(QColor(220, 220, 220), 1, Qt::DotLine));
    }
    
    // Vertical grid lines with labels
    for (int i = 0; i <= 10; ++i) {
        int x = leftMargin + i * plotWidth / 10;
        painter.drawLine(x, topMargin, x, topMargin + plotHeight);
        
        // X-axis tick labels
        if (maxPoints > 0 && i % 2 == 0) {
            int pointNum = (maxPoints * i) / 10;
            painter.setPen(QColor(44, 62, 80));
            painter.setFont(QFont("Microsoft YaHei UI", 8));
            painter.drawText(x - 20, topMargin + plotHeight + 5, 40, 15, 
                           Qt::AlignCenter, QString::number(pointNum));
            painter.setPen(QPen(QColor(220, 220, 220), 1, Qt::DotLine));
        }
    }
    
    // Draw title with gradient background
    QRect titleRect(leftMargin, 5, plotWidth, 20);
    QLinearGradient titleGradient(titleRect.topLeft(), titleRect.topRight());
    titleGradient.setColorAt(0, QColor(52, 152, 219, 50));
    titleGradient.setColorAt(0.5, QColor(52, 152, 219, 100));
    titleGradient.setColorAt(1, QColor(52, 152, 219, 50));
    painter.fillRect(titleRect, titleGradient);
    
    painter.setPen(QColor(52, 152, 219));
    painter.setFont(QFont("Microsoft YaHei UI", 11, QFont::Bold));
    painter.drawText(titleRect, Qt::AlignCenter, plotTitle);
    
    // Draw X-axis label - moved down to avoid overlap with border
    painter.setPen(QColor(44, 62, 80));
    painter.setFont(QFont("Microsoft YaHei UI", 9, QFont::Bold));
    painter.drawText(leftMargin, height() - 22, plotWidth, 20, 
                     Qt::AlignCenter, xAxisLabel);
    
    // Draw Y-axis label (rotated) - moved further left to avoid overlap
    painter.save();
    painter.setPen(QColor(44, 62, 80));
    painter.setFont(QFont("Microsoft YaHei UI", 9, QFont::Bold));
    painter.translate(12, topMargin + plotHeight / 2);
    painter.rotate(-90);
    painter.drawText(-60, 0, 120, 20, Qt::AlignCenter, yAxisLabel);
    painter.restore();
    
    // Find max data points across all channels
    maxPoints = 0;
    for (int i = 0; i < channels.size(); ++i) {
        if (channels[i].visible && channels[i].values.size() > maxPoints) {
            maxPoints = channels[i].values.size();
        }
    }
    
    if (maxPoints < 2) {
        // Draw "No Data" message
        painter.setPen(QColor(149, 165, 166));
        painter.setFont(QFont("Microsoft YaHei UI", 11, QFont::Bold));
        painter.drawText(leftMargin, topMargin, plotWidth, plotHeight,
                        Qt::AlignCenter, waitingMessage);
        return;
    }
    
    // Draw data lines with smooth curves
    for (int ch = 0; ch < channels.size(); ++ch) {
        if (!channels[ch].visible || channels[ch].values.size() < 2) {
            continue;
        }
        
        // Draw shadow for depth effect
        painter.setPen(QPen(channels[ch].color.darker(120), 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.setOpacity(0.3);
        
        QVector<QPointF> shadowPoints;
        for (int i = 0; i < channels[ch].values.size(); ++i) {
            QPointF p = dataToScreen(i, channels[ch].values[i], channels[ch].values.size());
            shadowPoints.append(QPointF(p.x() + 2, p.y() + 2));
        }
        
        for (int i = 0; i < shadowPoints.size() - 1; ++i) {
            painter.drawLine(shadowPoints[i], shadowPoints[i + 1]);
        }
        
        // Draw main line
        painter.setOpacity(1.0);
        painter.setPen(QPen(channels[ch].color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        
        QVector<QPointF> points;
        for (int i = 0; i < channels[ch].values.size(); ++i) {
            points.append(dataToScreen(i, channels[ch].values[i], channels[ch].values.size()));
        }
        
        // Draw lines
        for (int i = 0; i < points.size() - 1; ++i) {
            painter.drawLine(points[i], points[i + 1]);
        }
        
        // Draw points
        painter.setBrush(channels[ch].color);
        for (int i = 0; i < points.size(); ++i) {
            if (i % 5 == 0 || i == points.size() - 1) {  // Draw every 5th point and last point
                painter.drawEllipse(points[i], 3, 3);
            }
        }
    }
    
    // Draw legend with modern style
    int legendX = leftMargin + 10;
    int legendY = topMargin + 10;
    int legendSpacing = 22;
    
    // Draw legend background
    int legendHeight = 0;
    for (int i = 0; i < channels.size(); ++i) {
        if (!channels[i].values.isEmpty()) {
            legendHeight += legendSpacing;
        }
    }
    
    if (legendHeight > 0) {
        painter.setBrush(QColor(255, 255, 255, 230));
        painter.setPen(QPen(QColor(189, 195, 199), 1));
        painter.drawRoundedRect(legendX - 5, legendY - 5, 200, legendHeight + 5, 5, 5);
    }
    
    painter.setFont(QFont("Microsoft YaHei UI", 8, QFont::Bold));
    
    int currentY = legendY;
    for (int i = 0; i < channels.size(); ++i) {
        if (channels[i].values.isEmpty()) {
            continue;
        }
        
        // Draw color box with border
        painter.fillRect(legendX, currentY, 15, 12, channels[i].color);
        painter.setPen(QPen(channels[i].color.darker(120), 1));
        painter.drawRect(legendX, currentY, 15, 12);
        
        // Draw label with last value
        QString label = QString("%1: %2")
                        .arg(channels[i].name)
                        .arg(channels[i].values.last(), 0, 'f', 4);
        
        painter.setPen(QColor(44, 62, 80));
        painter.drawText(legendX + 20, currentY + 11, label);
        
        currentY += legendSpacing;
    }
}

void PlotWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    update();
}

void PlotWidget::setPlotTexts(const QString &title, const QString &yLabel, 
                              const QString &xLabel, const QString &waitingText)
{
    plotTitle = title;
    yAxisLabel = yLabel;
    xAxisLabel = xLabel;
    waitingMessage = waitingText;
    update();
}
