#include <iostream>

#include "Character.h"

namespace Character {

	class oVector {

	public:

		float X;
		float Y;

	};

	namespace Movement {

		namespace Text {

			std::string text = "";

			namespace Selection {

				int IndexA = 0;
				int IndexB = 0;

				int IndexHighest = 0;
				int IndexLowest = 0;

				namespace Set {

					void A(int Value) {
						IndexA = Value;
						if (IndexA > IndexB) {
							IndexHighest = IndexA;
							IndexLowest = IndexB;
							return;
						}
						IndexHighest = IndexB;
						IndexLowest = IndexA;
					}

					void B(int Value) {
						IndexB = Value;
						if (IndexA > IndexB) {
							IndexHighest = IndexA;
							IndexLowest = IndexB;
							return;
						}
						IndexHighest = IndexB;
						IndexLowest = IndexA;
					}

					void Both(int Value) {

						IndexA = Value;
						IndexB = Value;

						IndexHighest = Value;
						IndexLowest = Value;

					}

				}

				namespace Get {
						
					int A() {
						return IndexA;
					}

					int B() {
						return IndexB;
					}

					int Highest() {
						return IndexHighest;
					}

					int Lowest() {
						return IndexLowest;
					}

				}

			}

			namespace Get {

				int Line_Length(int Line) {
					int iCurrent_Character = 0;
					int iCurrent_Line = 0;

					while (true) {
						int iCurrent_Column = 0;
						while (true) {
							if (iCurrent_Character >= text.length()) {
								if (iCurrent_Line < Line) {
									return 0;
								}
								return iCurrent_Column;
							}
							if (text.at(iCurrent_Character) == '\n') {
								iCurrent_Line++;
								iCurrent_Character++;
								break;
							}
							iCurrent_Column++;
							iCurrent_Character++;
						}
						if (iCurrent_Line > Line) {
							return iCurrent_Column;
						}
					}
				}
				int Index_From_Point(int Line, int Column) {
					int iIndex = 0;

					for (int line = 0; line < Line; line++) {
						iIndex += Line_Length(line) + 1;
					}

					iIndex += Column;

					return iIndex;
				}
				int Line_From_Index(int Index) {
					int Line = 0;
					int Column = Index;

					while (true) {
						if (Column >= Line_Length(Line)) {
							Column -= (Line_Length(Line) + 1);
							Line++;
						}
						else {
							break;
						}
					}
					return Line;
				}
				int Column_From_Index(int Index) {
					int Line = 0;
					int Column = Index;

					while (true) {
						if (Column >= Line_Length(Line)) {
							Column -= (Line_Length(Line) + 1);
							Line++;
						}
						else {
							break;
						}
					}
					return Column;
				}
				std::string Character_From_Index(int Index) {
					return std::string(1, text.at(Index));
				}
				std::string Text() {
					return text;
				}

			}

			namespace Cut {

				void Selection() {

					text.erase(Selection::IndexLowest, Selection::IndexHighest);

				}

				void Front() {

					text.erase(Selection::IndexHighest, 1);

				}

				void Back() {

					text.erase(Selection::Get::Lowest() - 1, 1);

					Selection::Set::Both(Selection::Get::Lowest() - 1);

				}

			}

			void Type(std::string str) {

				text.insert(Selection::Get::Lowest(), str);
				Selection::Set::Both(Selection::Get::Lowest() + 1);

			}

		}

	}

}