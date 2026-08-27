#include <SFML/Graphics.hpp>

int main() {
	// Crear una ventana de 800x600 con el título del proyecto
	sf::RenderWindow window(sf::VideoMode(800, 600), "Tetris - Proyecto I EIF207");
	
	// Bucle principal del juego
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}
		
		// Limpiar la pantalla con color negro
		window.clear(sf::Color::Black);
		
		// Dibujar un círculo de prueba
		sf::CircleShape shape(50.f);
		shape.setFillColor(sf::Color::Green);
		shape.setPosition(375.f, 275.f);
		window.draw(shape);
		
		// Mostrar todo en la pantalla
		window.display();
	}
	
	return 0;
}
