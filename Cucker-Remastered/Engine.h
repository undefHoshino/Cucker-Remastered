#pragma once
#include <map>
#include "Logger.h"
#include "ConsoleSetting.h"
#include "Exceptions.h"

class ConsoleEngine {
public:
	class Component {
	protected:
		ConsoleEngine* parent = nullptr;
	public:
		virtual void Init(ConsoleEngine* source, void* args) {
			parent = source;
		}
		void SetParent(ConsoleEngine* source){
			parent = source;
		}
		virtual void Start() {};
	};

protected:
	Logger logger;
	std::map<const type_info*, Component*> component;

public:
	ConsoleEngine():logger("ConsoleEngine") {};

	virtual void Initialization();
	virtual void Run();
	void Free();

	~ConsoleEngine();
protected:
	// Register interface and Navigate
	virtual void Register() = 0;
public:
	template<class Com>
	Com* Use(void* args = 0) {
		auto com = new Com();
		com->Init(this, args);
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

	bool ExistComponent(const std::type_info& type);
};

using EngineComponent = ConsoleEngine::Component;