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

private:
	std::shared_ptr<Model3D> model;
	std::shared_ptr<class Camera> perspective;

	QTimer* timer;
};