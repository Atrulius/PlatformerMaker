#include <iostream>

namespace Character {

	class oVector;

	namespace Movement {

		namespace Text {

			void Type(std::string str);

			namespace Selection {

				namespace Set {

					void A(int Value);

					void B(int Value);

					void Both(int Value);

				}

				namespace Get {

					int A();

					int B();

					int Highest();

					int Lowest();

				}

			}

			namespace Get {

				int Line_Length(int Line);
				int Index_From_Point(int Line, int Column);
				int Line_From_Index(int Index);
				int Column_From_Index(int Index);
				std::string Character_From_Index(int Index);
				std::string Text();

			}

			namespace Cut {

				void Selection();
				void Front();
				void Back();

			}

		}

	}

}