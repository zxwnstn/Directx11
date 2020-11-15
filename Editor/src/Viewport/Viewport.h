#pragma once


class Viewport : public QWidget
{
	Q_OBJECT
public:
	Viewport(QWidget* parent = nullptr);
	~Viewport();

public:
	void Init();

private slots:
	void Update();
	void Render();

protected:
	void keyPressEvent(QKeyEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
	void paintEvent(QPaintEvent* event) override;


private:

	QTimer* timer;
};