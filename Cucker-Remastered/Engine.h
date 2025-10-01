#pragma once
#include <iostream>
#include <map>
#include <io.h>       // For _setmode()
#include "Date.h"
#include "Converter.h"
#include "Interface.h"
#include "InputHandler.h"
#include "ConsoleSetting.h"
#include "Exceptions.h"

class ConsoleEngine {
public:
	class Component {
	protected:
		ConsoleEngine* parent = nullptr;
	public:
		virtual void Init(ConsoleEngine& source) {
			parent = &source;
		}
	};

protected:
	Logger logger;
	std::map<const type_info*, Component*> component;

public:
	ConsoleEngine() {};

	virtual void Initialization() {
		logger.SetClassName("ConsoleEngine");
	}
	virtual void Run() = 0;

protected:
	bool ExistComponent(const std::type_info& type) {
		return component.find(&type) != component.end();
	}

	// Register interface and Navigate
	virtual void Register() = 0;

public:
	template<class Com>
	Com* Use(Com* com) {
		com->Init(*this);
		component[&typeid(Com)] = com;
		return com;
	}

	template <typename Com>
	Com& GetComponent() {
		if (!ExistComponent(typeid(Com))) {
			throw ReportException("Component (name: ", typeid(Com).name(), ", raw: ", typeid(Com).raw_name(), ") doesn't exist");
		}
		return *static_cast<Com*>(component[&typeid(Com)]);
	}

	void Free() {
		for (auto& ptr : component) {
			delete ptr.second;
			ptr.second = nullptr;
		}
		component.clear();
	}

	~ConsoleEngine() {
		Free();
	}
};