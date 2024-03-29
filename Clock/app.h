#pragma once

#include"resource.h"

#include<Windows.h>
#include<d2d1.h>
#include<d2d1helper.h>
#include <sstream>
#include <unordered_map>
#include <map>
#include <set>
#include <string>
#include <stack>
#include <memory>
#include <vector>


	namespace external
	{
		namespace physical
		{
			namespace input_output
			{
				// RAS
			}

			namespace input
			{
				// RAS
			}

			namespace output
			{
				// RAS
			}

			namespace user
			{
				// RAS
			}
		}
	}

	namespace abstraction
	{
		namespace data
		{
			class Data
			{
			public:
				virtual ~Data() = default;
			};

			class InputData : public Data
			{
			public:
				virtual ~InputData() = default;
			};

			class OutputData : public Data
			{
			public:
				virtual ~OutputData() = default;
			};

			class Shape
			{
			public:
				virtual~Shape() = default;
				virtual std::string getName()const = 0;
			};


			namespace command
			{
				class Command
				{
				public:
					virtual ~Command() = default;

					void undo() { return undoImpl(); }
					void execute()
					{
						checkPreConditionImpl();
						executeImpl();
						checkPostConditionImpl();
					}
					Command* clone() const { return cloneImpl(); }
					virtual void deallocate() { delete this; }
					const char* getHelpMessage() const { return getHelpMessageImpl(); }

				protected:
					Command() = default;
					Command(const Command&) = default;

					virtual void undoImpl() noexcept = 0;
					virtual void executeImpl() noexcept = 0;
					virtual Command* cloneImpl() const noexcept = 0;

					virtual void checkPostConditionImpl() const {}
					virtual void checkPreConditionImpl() const {}
					virtual const char* getHelpMessageImpl() const noexcept = 0;

				private:
					Command(Command&&) = delete;
					Command& operator=(const Command&) = delete;
					Command& operator=(Command&&) = delete;
				};

				/*
					must go in the Algoritm namespace
				*/
				inline void deallocate(Command* cmd)
				{
					if (cmd)
						cmd->deallocate();
				}

				using unique_command_ptr = std::unique_ptr<Command, decltype(&deallocate)>;

				inline auto make_unique_command_ptr(Command* c)
				{
					return unique_command_ptr{ c, &deallocate };
				}

				// 2: Creational Pattern: Abstract Factory
				class CommandFactory
				{
				public:
					virtual ~CommandFactory() = default;
					virtual unique_command_ptr create() const
					{
						return createImpl();
					}

				protected:
					CommandFactory() = default;
					CommandFactory(const CommandFactory&) = default;

				protected:
					virtual unique_command_ptr createImpl() const = 0;

				private:
					CommandFactory(CommandFactory&&) = delete;
					CommandFactory& operator=(const CommandFactory&) = delete;
					CommandFactory& operator=(CommandFactory&&) = delete;
				};
			}

			namespace exception
			{
				class Exception
				{
				public:
					Exception(const std::string& message) : m_msg{ message } {}
					const std::string& what() const { return m_msg; }

				private:
					std::string m_msg;
				};
			}

		} // namespace _system

		namespace logic
		{
			namespace business
			{
				// Class Command;
				class IManager
				{
				public:
					virtual ~IManager() = default;
					virtual void execute(data::command::Command* data) noexcept = 0;
					// virtual const char *execute(data::InputData *data) noexcept = 0;
				};
			}
			namespace algorithm
			{
				class IShapeFactory 
				{
				public:
					virtual~IShapeFactory() = default;
					virtual std::shared_ptr<data::Shape> create(const std::string& type) = 0;
				};

			}

			namespace service
			{
				class IService
				{
				public:
					virtual ~IService() = default;
					virtual std::string getName() const = 0;
					virtual abstraction::data::OutputData* transform(std::shared_ptr<abstraction::data::InputData> d) = 0;
					virtual const std::string getServiceDescription() const = 0;
					virtual const std::string getServiceLocalisation() const = 0;
				};
			}

		} // namespace logic

		namespace boundary
		{
			namespace user_interaction
			{
				class IUserInteraction
				{
				public:
					virtual ~IUserInteraction() = default;
					virtual void sendInput() = 0;
					virtual void sendOutput(const char*) = 0;
					virtual void sendOutput(std::shared_ptr<abstraction::data::Data>d) = 0;

				};
			}

			namespace proxy
			{
				class Observer
				{
				public:
					Observer(const std::string& n) : name{ n } {}
					const std::string& getName() const { return name; }
					virtual void notify(std::shared_ptr<abstraction::data::Data> d) { notifyImpl(d); };
					virtual ~Observer() = default;

				private:
					virtual void notifyImpl(std::shared_ptr<abstraction::data::Data> d) = 0;

				private:
					std::string name;
				};
			}

			namespace device_input_output
			{

			}
		}

		namespace control
		{
			namespace coordinator
			{
			} // namespace coordinator

			namespace state_dependent_control
			{

			} // namespace state_dependent_control

			namespace timer
			{
				class Timer
				{
				public:
					virtual~Timer() = default;
				};

			}
		} // namespace control
	} // namespace abstraction

	namespace service_system
	{
		namespace logger
		{
			namespace logger_data_abstraction
			{
				class LoggerData
				{
				};
			}
			class Logger : public abstraction::logic::service::IService
			{
				// to be add in the concrete Logger
				static const std::string name;

			public:
				virtual ~Logger() = default;
				virtual void log(const char*) = 0;

			public:
				virtual std::string getName() const noexcept override { return "Logger"; };
				const std::string getServiceDescription() const override { return "This Class is use to log"; }
				const std::string getServiceLocalisation() const override { return "Located somewhere"; }
			};
		}

		namespace publisher
		{
			namespace publisher_data_abstraction
			{
				class PublisherData : public abstraction::data::Data
				{
				public:
				};
			}

			class Publisher : public abstraction::logic::service::IService
			{
				static const std::string name;
				class PublisherImpl;

			public:
				Publisher();
				void subscribe(const std::string& eventName, std::unique_ptr<abstraction::boundary::proxy::Observer> observer);
				void unsubscribe(const std::string& eventName, const std::string& observerName);
				std::string getName() const noexcept override { return name; }
				const std::string getServiceDescription() const override { return "This Class is used for subscription"; }
				const std::string getServiceLocalisation() const override { return "Located somewhere"; }
				abstraction::data::OutputData* transform(std::shared_ptr<abstraction::data::InputData> f) override
				{
					return nullptr;
				};

				virtual ~Publisher();
				void notify(const std::string& eventName, std::shared_ptr<abstraction::data::Data>) const;
				void registerEvent(const std::string& eventName);

			private:
				std::unique_ptr<PublisherImpl> impl;
			};
		}

		namespace tokenizer
		{
			namespace data {
				class TokenizerInputData : abstraction::data::InputData
				{
				public:
					TokenizerInputData(const std::string& str, char token)
						: m_sData{ str }, m_cToken{ token }{}

					~TokenizerInputData()
					{
					}
					const std::string& getData()const { return m_sData; }
					char getToken()const { return m_cToken; }

				private:
					std::string m_sData;
					char m_cToken;
				};


				class TokenizerOutputData : public abstraction::data::OutputData
				{
					using Token = std::string;
					using Tokens = std::vector<std::string>;
					using const_iterator = Tokens::const_iterator;

				public:
					TokenizerOutputData(const std::vector<std::string> &strs)
						:m_tokens{ strs } {}

					~TokenizerOutputData()
					{
					}
					const_iterator begin() const { return m_tokens.cbegin(); }
					const_iterator end() const { return m_tokens.cend(); }
					const Token& operator[](size_t i) const { return m_tokens[i]; }
					size_t size() const { return m_tokens.size(); }

				private:
					Tokens m_tokens;
				};

			}
			namespace boundary
			{
				namespace proxy
				{

				} // namespace proxy
			}

			namespace logic
			{
				namespace business
				{

				} // namespace business

				namespace service
				{
					class TokenizerService : public abstraction::logic::service::IService
					{
						static const std::string name; // = "tokenizer";
					public:
						TokenizerService()=default;

						std::string getName() const noexcept override { return name; }
						const std::string getServiceDescription() const override { return "tokenize the string"; }
						const std::string getServiceLocalisation() const override { return "located somewhere"; }

						abstraction::data::OutputData* transform(std::shared_ptr<abstraction::data::InputData> d) override;
						~TokenizerService();

					private:
						TokenizerService(const TokenizerService&) = delete;
						TokenizerService(TokenizerService&&) = delete;
						TokenizerService& operator=(const TokenizerService&) = delete;
						TokenizerService& operator=(TokenizerService&&) = delete;
					};
				}
			}

		}
	}

	namespace broker_system
	{
		namespace white_page
		{
			class BrokerForwarder
			{
			public:
				static BrokerForwarder& getInstance()
				{
					static BrokerForwarder instance;
					return instance;
				}
				~BrokerForwarder() = default;
				bool registerService(std::unique_ptr<abstraction::logic::service::IService> s) noexcept
				{
					return false;
				}
				abstraction::data::OutputData* forward(const std::string& serviceName, const abstraction::data::InputData& data) const noexcept;
			private:
				BrokerForwarder() : m_services{}
				{					
					m_services.insert(std::make_pair("tokenizer", std::make_unique<service_system::tokenizer::logic::service::TokenizerService>()));
				}
				std::unordered_map<std::string, std::unique_ptr<abstraction::logic::service::IService>> m_services;

			private:
				BrokerForwarder(const BrokerForwarder&) = delete;
				BrokerForwarder(BrokerForwarder&&) = delete;
				BrokerForwarder& operator=(const BrokerForwarder&) = delete;
				BrokerForwarder& operator=(BrokerForwarder&&) = delete;
			};
			class BrokerHandler
			{
			public:
				static BrokerHandler& getInstance()
				{
					static BrokerHandler instance;
					return instance;
				}
				~BrokerHandler() = default;
				bool registerService(std::shared_ptr<abstraction::logic::service::IService> s) noexcept;
				std::shared_ptr<abstraction::logic::service::IService> getService(const std::string& serviceName) const noexcept;
			private:
				BrokerHandler() : m_services{}
				{					
					m_services.insert(std::make_pair("tokenizer", std::make_unique<service_system::tokenizer::logic::service::TokenizerService>()));
				}
				std::unordered_map<std::string, std::shared_ptr<abstraction::logic::service::IService>> m_services;

			private:
				BrokerHandler(const BrokerHandler&) = delete;
				BrokerHandler(BrokerHandler&&) = delete;
				BrokerHandler& operator=(const BrokerHandler&) = delete;
				BrokerHandler& operator=(BrokerHandler&&) = delete;
			};

		}

		namespace yellow_page
		{
			enum class RegisteredServiceType
			{
				TYPE1,
				TYPE2,
				TYPE3,
			};

			class BrokerDiscoverer
			{
			public:
				static BrokerDiscoverer& getInstance()
				{
					static BrokerDiscoverer instance;
					return instance;
				}
				~BrokerDiscoverer() = default;
				bool registerService(RegisteredServiceType serviceType, std::unique_ptr<abstraction::logic::service::IService> s) noexcept;
				std::vector<std::string> getService(RegisteredServiceType serviceType) const;
				std::shared_ptr<abstraction::data::OutputData> forward(const std::string& serviceName, const abstraction::data::InputData& data) const noexcept;

			private:
				BrokerDiscoverer() : m_services{}
				{
				}
				std::vector<std::pair<RegisteredServiceType, std::unique_ptr<abstraction::logic::service::IService>>> m_services;

			private:
				BrokerDiscoverer(const BrokerDiscoverer&) = delete;
				BrokerDiscoverer(BrokerDiscoverer&&) = delete;
				BrokerDiscoverer& operator=(const BrokerDiscoverer&) = delete;
				BrokerDiscoverer& operator=(BrokerDiscoverer&&) = delete;
			};
		}
	}

	namespace app
	{
#define CLOCK_TIMER_ID 1

		namespace data_abstraction
		{
			class UpdateCommand : public abstraction::data::command::Command
			{
			public:
				UpdateCommand(const std::string& t)
					: Command(), 
					m_time{ t } {};

				UpdateCommand(const UpdateCommand& dC)
					:Command(dC),
					m_time{ dC.m_time } {}

				~UpdateCommand() = default;

			protected:
				virtual void undoImpl()noexcept override;
				virtual void executeImpl()noexcept override;
				virtual UpdateCommand* cloneImpl()const noexcept override { return new UpdateCommand{ *this }; }

				//virtual	void checkPostConditionImpl()const override {};
				//virtual	void checkPreConditionImpl()const override {};
				virtual const char* getHelpMessageImpl()const noexcept override { return "Rotate the Hand"; };

			private:
				std::string m_time;

			private:
				UpdateCommand(UpdateCommand&&) = delete;
				UpdateCommand& operator=(const UpdateCommand&) = delete;
				UpdateCommand& operator=(UpdateCommand&&) = delete;
			};
		}

		namespace logic
		{
			namespace service
			{

			}
		}

		// Model
		namespace server_subsystem
		{
			namespace data_abstraction
			{
				enum class ErrorType
				{
					EMPTY,
					TOO_FEW_ARGUMENT
				};

				class Rectangle : public abstraction::data::Shape
				{
				public:
					Rectangle() {
						m_rec.left = 0.0f;
						m_rec.top = 0.0f;
						m_rec.right = 0.0f;
						m_rec.bottom = 0.0f;
					};

					Rectangle(const Rectangle& other) {
						m_rec.left = other.m_rec.left;
						m_rec.top = other.m_rec.top;
						m_rec.right = other.m_rec.right;
						m_rec.bottom = other.m_rec.bottom;
					}
					Rectangle(float left, float top, float right, float bottom)
					{
						m_rec.left = left;
						m_rec.top = top;
						m_rec.right = right;
						m_rec.bottom = bottom;
					}
					std::string getName()const override { return "Rectangle"; }

					bool operator==(const Rectangle& other) const
					{
						return m_rec.left == other.m_rec.left &&
						m_rec.top == other.m_rec.top &&
						m_rec.right == other.m_rec.right &&
						m_rec.bottom == other.m_rec.bottom;

					}

					float getLeft()const { return m_rec.left; }
					float getTop()const { return m_rec.top; }
					float getRight()const { return m_rec.right; }
					float getBottom()const { return m_rec.bottom; }

					void setLeft(float l) {  m_rec.left=l; }
					void setTop(float t) {  m_rec.top=t; }
					void setRight(float r) {  m_rec.right=r; }
					void setBottom(float b) {  m_rec.bottom=b; }

					~Rectangle() = default;
					std::ostream& Print(std::ostream& os) const
					{
						return os 
							<< "(" << m_rec.left << "," << m_rec.top <<"," 
							<< m_rec.right <<"," << m_rec.bottom << ")";
					}

				private:
					D2D1_RECT_F m_rec;
				};
				//bool operator==(const Rectangle& lr, const Rectangle& hr)
				//{
				//	return lr.getLeft() == hr.getLeft()
				//		&& lr.getTop() == hr.getTop()
				//		&& lr.getRight() == hr.getRight()
				//		&& lr.getBottom() == hr.getBottom();
				//}

				class ModelOutputData : public abstraction::data::OutputData
				{
				public:
					ModelOutputData(const Rectangle& r, float angle) : m_rect(r), m_fAngle{angle}{}

					~ModelOutputData() = default;

					const Rectangle& getRectangle()const { return m_rect; }
					float getAngle()const { return m_fAngle; }

				private:
					Rectangle m_rect;
					float m_fAngle;
				};

				enum class ShapeID
				{
					HOURS,
					SECOND,
					MINUTS,
					CIRCLE
				};

				class ModelProxyImpl
				{
					using Model = std::map<std::string, std::shared_ptr<abstraction::data::Shape>>;
				public:
					using const_iterator = Model::const_iterator;
					using const_reference = Model::const_reference;

				public:
					~ModelProxyImpl() = default;
					ModelProxyImpl();

				public:
					const_iterator cbegin()const { return m_data.cbegin(); }
					const_iterator cend()const { return m_data.cend(); }
					std::shared_ptr<abstraction::data::Shape> getShape(const std::string& name)const;

				private:
					void initialize()noexcept;
				private:
					Model m_data;
				private:
					ModelProxyImpl(const ModelProxyImpl&) = delete;
					ModelProxyImpl(ModelProxyImpl&&) = delete;
					ModelProxyImpl& operator=(const ModelProxyImpl&) = delete;
					ModelProxyImpl& operator=(ModelProxyImpl&&) = delete;
				};
			}

			namespace boundary
			{
				namespace user_interaction
				{
					// RAS
				}

				namespace proxy
				{
					class ModelObserver : public abstraction::boundary::proxy::Observer
					{
					public:
						explicit ModelObserver(abstraction::boundary::user_interaction::IUserInteraction& ui)
							: Observer("ModelObserver"),
							m_ui{ ui }
						{}

					private:
						void notifyImpl(std::shared_ptr<abstraction::data::Data>) override;

					private:
						abstraction::boundary::user_interaction::IUserInteraction& m_ui;
					};

					class ModelProxy :/*private data_abstraction::AdamProxyImpl,*/ protected service_system::publisher::Publisher
					{
					public:
						static const std::string resultAvailable;
						static const std::string adamError;

					public:
						using Publisher::subscribe;
						using Publisher::unsubscribe;

					public:
						static ModelProxy& getInstance();

						void update(const std::string& time, bool notify)noexcept;
					private:
						ModelProxy();

					private:
						data_abstraction::ModelProxyImpl m_data_;
						using Model = std::map<std::string, data_abstraction::Rectangle>;
						Model m_data;
					};
				}

				namespace device_input_output
				{
					// RAS
				}
			}

			namespace control
			{
				namespace coordinator
				{
					// 5: Behavioral Pattern: Strategy
					class ServerCoordinator
					{
						class ServerCoordinatorImpl;
						class UndoRedoStackStrategy;
						class UndoRedoListStrategyVector;
						class UndoRedoListStrategy;

					public:
						enum class UndoRedoStrategy
						{
							ListStrategy,
							StackStrategy,
							ListStrategyVector
						};

					public:
						ServerCoordinator(UndoRedoStrategy st = UndoRedoStrategy::StackStrategy);
						~ServerCoordinator();

						void executeCommand(abstraction::data::command::unique_command_ptr c);
						size_t getUndoSize() const;
						size_t getRedoSize() const;

						void undo();
						void redo();
						void update();
					private:
						std::unique_ptr<ServerCoordinatorImpl> pimpl_;

					};
				}

				namespace state_dependent_control
				{
					// RAS
				}

				namespace timer
				{
					// RAS
				}

			}

			namespace logic
			{
				namespace business_logic
				{

				}

				namespace algorithm
				{
					// RAS
				}

				namespace service
				{
					// RAS
				}
			}
		}

		namespace client_subsystem
		{
			namespace view
			{
				namespace boundary
				{
					namespace user_interaction
					{
						class UserInterface : public abstraction::boundary::user_interaction::IUserInteraction, protected service_system::publisher::Publisher
						{
						public:
							static const char* InputEntered;

						public:
							UserInterface() { Publisher::registerEvent(InputEntered); }
							virtual ~UserInterface() = default;
							// virtual void sendInput() override;
							// virtual void sendOutput(const char *) override;

						public:
							using Publisher::notify;
							using Publisher::subscribe;
							using Publisher::unsubscribe;
						};
					}
				}
				
			}

			namespace controller
			{
				namespace data
				{
					class CommandRepository
					{
						class CommandRepositoryImpl;

					public:
						static CommandRepository& getInstance();

						void registerCommand(const std::string& name, abstraction::data::command::unique_command_ptr c);
						abstraction::data::command::unique_command_ptr deregisterCommand(const std::string& name);
						size_t count() const;
						abstraction::data::command::unique_command_ptr getCommandByName(const std::string& name) const;
						bool hasKey(const std::string& s) const;
						std::set<std::string> getAllCommandNames() const;
						void printHelp(const std::string& command, std::ostream&) const;
						void clearAllCommands();

					private:
						CommandRepository();
						~CommandRepository();
						std::unique_ptr<CommandRepositoryImpl> pimpl_;

					private:
						CommandRepository(CommandRepository&) = delete;
						CommandRepository(CommandRepository&&) = delete;
						CommandRepository& operator=(CommandRepository&) = delete;
						CommandRepository& operator=(CommandRepository&&) = delete;
					};

				} // namespace data

				namespace control
				{
					namespace coordinator
					{
						class ClientCoordinator
						{
						public:
							ClientCoordinator(/* args */) :m_server_coordinator{} {}
							~ClientCoordinator() {}
							void executeCommand(abstraction::data::command::unique_command_ptr c);
							void update();

						private:
							server_subsystem::control::coordinator::ServerCoordinator m_server_coordinator;
						};
					} // namespace coordinator

					namespace state_dependent_control
					{
						class CommandDispatcher
						{
							class CommandDispatcherImpl;

						public:
							~CommandDispatcher();
							static CommandDispatcher& getInstance(client_subsystem::view::boundary::user_interaction::UserInterface& ui);
							void commandEntered(const std::string& command, const std::string& sender);
						private:
							explicit CommandDispatcher(client_subsystem::view::boundary::user_interaction::UserInterface& ui);
							std::unique_ptr<CommandDispatcherImpl> pimpl_;

						private:
							CommandDispatcher(const CommandDispatcher&) = delete;
							CommandDispatcher(CommandDispatcher&&) = delete;
							CommandDispatcher& operator=(const CommandDispatcher&) = delete;
							CommandDispatcher& operator=(CommandDispatcher&&) = delete;
						};

					} // namespace state_dependent_control

					namespace timer
					{
						class ClockTimer : public abstraction::control::timer::Timer
						{
						public:
							ClockTimer(HWND hWnd, int id, UINT period) :m_hwnd{ hWnd }
							{
								//SetTimer(hWnd, id, period,TimerProc);
								SetTimer(hWnd, id, period,NULL);
							}

							~ClockTimer()
							{
							}

						private:
							static void CALLBACK TimerProc(HWND hWnd, UINT uTimerMsg, UINT uTimerID, DWORD dwTime);
						private:
							HWND m_hwnd;
						};

					}
				} // namespace control
				namespace logic
				{
					namespace business_logic
					{
						// RAS
					}

					namespace algorithm
					{
						// RAS
					}

					namespace service
					{
						// RAS
					}
				}
			} // namespace controller

			namespace view
			{
				namespace logic {
					namespace algorithm {
						template <class Interface>
						inline void SafeRelease(Interface** ppInterfaceToRelease) {
							if (*ppInterfaceToRelease != nullptr) {
								(*ppInterfaceToRelease)->Release();
								(*ppInterfaceToRelease) = nullptr;
							}
						}
					}
				}
				namespace data
				{
					static LPCWSTR lpszClassName = L"ClockClass";
					static HMENU lpszMenuName = NULL;
					static LPCWSTR lpszAppName = L"Sample App";
					static INT defaultAppWidth = 400;
					static INT defaultAppHeight = 400;
					static INT defaultAppPosX = 100;
					static INT defaultAppPosY = 100;
					//HINSTANCE hInst;
					class UserInterfaceIntputData : public abstraction::data::InputData
					{
					public:
						UserInterfaceIntputData(const std::string& userInput, const std::string& Sender = "")
							: uii{ userInput },
							sender_{ Sender } {}
						const std::string& getData() const { return uii; }
						const std::string& getSender() const { return sender_; }

					private:
						std::string uii;
						std::string sender_;
					};
					class UserInterfaceOutputData : public abstraction::data::OutputData
					{
					public:
						UserInterfaceOutputData(const std::string& userInput) : uii{ userInput } {}
						const std::string& getEventData() const { return uii; }

					private:
						std::string uii;
					};

					// for all the data needed in the Win class.
					class WinImpl
					{
					public:
						WinImpl(HWND hwnd);
						virtual~WinImpl();

						HRESULT createDeviceIndependentResource();
						HRESULT createDeviceDependentResource();
						void discardDeviceResources();

						HWND hwnd;
						ID2D1Factory* m_pDirect2dFactory;
						ID2D1HwndRenderTarget* m_pRenderTarget;
						ID2D1SolidColorBrush* m_pLightSlateGrayBrush;
						ID2D1SolidColorBrush* m_pCornflowerBlueBrush;
					};


				} // namespace data

				namespace boundary
				{
					namespace user_interaction
					{
						namespace cli
						{
							class CustomerInteraction : public UserInterface
							{
							public:
								CustomerInteraction(std::istream& is, std::ostream& os) : m_is{ is }, m_os{ os } {}
								~CustomerInteraction() = default;
								void run();

							private:
								void sendInput() override;
								void sendOutput(const char* msg) override;
								void sendOutput(std::shared_ptr<abstraction::data::Data>d)override;

							private:
								std::istream& m_is;
								std::ostream& m_os;

							private:
								CustomerInteraction(const CustomerInteraction&) = delete;
								CustomerInteraction(CustomerInteraction&&) = delete;
								CustomerInteraction& operator=(const CustomerInteraction&) = delete;
								CustomerInteraction& operator=(CustomerInteraction&&) = delete;
							};
						} // namespace cli

						namespace gui {
							enum WindowID
							{
								BUTTON_UNDO,
								BUTTON_REDO,
								LABEL_EMPTY,

								//------------Popup-----------------//
								HTML_POPUP_MENU,
								BUTTON_OPEN_FILTER,
							};
#define MAX_BYTE 256

							/*
								Inherite privately from the dataAquisition class
								to gain access to the protected data...
							*/
							class Win : public UserInterface
							{
							public:
								Win();
								~Win();

								HRESULT init();
								void run();

								void sendInput() override;
								void sendOutput(const char* msg) override;
								void sendOutput(std::shared_ptr<abstraction::data::Data>d)override;
								HWND Window() const { return m_hwnd; }

							private:
								static LRESULT CALLBACK	WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
								HRESULT OnRender(const abstraction::data::Shape&,float angle);
								HRESULT OnHandRender(const server_subsystem::data_abstraction::Rectangle& rec, float angle);
								void OnResize(UINT width, UINT height);

							private:
								HWND m_hwnd;
								data::WinImpl m_data;

								//D2D1_RECT_F m_rectangle;
							};
						}
					}	  // namespace user_interaction

					namespace proxy
					{
						class UserInterfaceObserver : public abstraction::boundary::proxy::Observer
						{
						public:
							explicit UserInterfaceObserver(controller::control::state_dependent_control::CommandDispatcher& ce)
								: Observer{ "UserInterfaceObserver" },
								m_ce{ ce }
							{};

						private:
							void notifyImpl(std::shared_ptr<abstraction::data::Data>) override;

							controller::control::state_dependent_control::CommandDispatcher& m_ce;
						};
					} // namespace proxy
				}

			} // namespace view
		}

		class Facade
		{
		public:
			Facade(/*HINSTANCE h*/)/* :m_hinstance{h}*/{};
			static const char* getFacadeDescription() { return "the Facade of my system"; }
			void run();

		protected:
			virtual void start() {};
		private :
			//HINSTANCE m_hinstance;
		};
	}

