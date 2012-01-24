
void parseData() {
	ifstream bus_data_file("data/set_default/bus_data.txt");
	ifstream cost_data_file("data/set_default/cost_cofficients.txt");
	ifstream power_limits_file("data/set_default/generator_limits.txt");
	ifstream line_data_file("data/set_default/line_data.txt");

	TiXmlDocument lines_data;
	TiXmlDeclaration * decl = new TiXmlDeclaration("1.0", "utf-8", "");
	TiXmlElement * root = new TiXmlElement("lines-data");
	TiXmlElement * lines = new TiXmlElement("lines");
	lines_data.LinkEndChild(decl);
	lines_data.LinkEndChild(root);
	root->LinkEndChild(lines);

	while ( ! line_data_file.eof()) {
		char str[128];
		TiXmlElement * element = new TiXmlElement("line");

		line_data_file.getline(str, sizeof(str));
		char from[10], to[10], R[10], X[10], b[10], t[10];
		sscanf(str, "%s\t%s\t%s\t%s\t%s\t%s\n", &from, &to, &R, &X, &b, &t);


		element->LinkEndChild(new TiXmlElement("bus-from"))->LinkEndChild(new TiXmlText(from));
		element->LinkEndChild(new TiXmlElement("bus-to"))->LinkEndChild(new TiXmlText(to));
		element->LinkEndChild(new TiXmlElement("resistance"))
						->LinkEndChild(new TiXmlElement("active"))->LinkEndChild(new TiXmlText(R))
						->Parent()->Parent()
						->LinkEndChild(new TiXmlElement("reactive"))->LinkEndChild(new TiXmlText(X));
		element->LinkEndChild(new TiXmlElement("halfB"))->LinkEndChild(new TiXmlText(b));
		element->LinkEndChild(new TiXmlElement("tap-in-bus"))->LinkEndChild(new TiXmlText(t));



		lines->LinkEndChild(element);
	}
	lines_data.SaveFile("lines_data.xml");


//	while( ! bus_data_file.eof()) {
//		char str[128];
//		TiXmlElement * element = new TiXmlElement("bus");
//
//		bus_data_file.getline(str, sizeof(str));
//
//		char no[100], code[100], V[100], Degree[100], Pload[100], Qload[100], Pgen[100], Qgen[100], Qmin[100], Qmax[100], I[100];
//		sscanf(str, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
//					&no, &code,
//					&V, &Degree,
//					&Pload,
//					&Qload,
//					&Pgen,
//					&Qgen,
//					&Qmin,
//					&Qmax,
//					&I
//			);
//
//		element->LinkEndChild(new TiXmlElement("no"))->LinkEndChild(new TiXmlText(no));
//		element->LinkEndChild(new TiXmlElement("code"))->LinkEndChild(new TiXmlText(code));
//		element->LinkEndChild(new TiXmlElement("voltage"))
//					->LinkEndChild(new TiXmlElement("value"))
//						->LinkEndChild(new TiXmlText(V))
//							->Parent()->Parent()
//					->LinkEndChild(new TiXmlElement("min-value"))
//						->LinkEndChild(new TiXmlText("0.9"))
//							->Parent()->Parent()
//					->LinkEndChild(new TiXmlElement("max-value"))
//						->LinkEndChild(new TiXmlText("1.15"))
//							->Parent()->Parent()
//							;
//
//		element->LinkEndChild(new TiXmlElement("angle"))
//					->LinkEndChild(new TiXmlElement("value"))
//						->LinkEndChild(new TiXmlText(Degree))
//							->Parent()->Parent()
//					->LinkEndChild(new TiXmlElement("min-value"))
//						->LinkEndChild(new TiXmlText("-45"))
//							->Parent()->Parent()
//					->LinkEndChild(new TiXmlElement("max-value"))
//						->LinkEndChild(new TiXmlText("45"))
//							->Parent()->Parent()
//							;
//
//		element->LinkEndChild(new TiXmlElement("power-load"))
//					->LinkEndChild(new TiXmlElement("active"))
//						->LinkEndChild(new TiXmlElement("value"))
//							->LinkEndChild(new TiXmlText(Pload))
//								->Parent()->Parent()->Parent()
//					->LinkEndChild(new TiXmlElement("reactive"))
//						->LinkEndChild(new TiXmlElement("value"))
//							->LinkEndChild(new TiXmlText(Qload))
//							;
//
//
//		if (code[0] == * "1" || code[0] == * "2") {
//			element->LinkEndChild(new TiXmlElement("power-gen"))
//						->LinkEndChild(new TiXmlElement("active"))
//							->LinkEndChild(new TiXmlElement("value"))
//								->LinkEndChild(new TiXmlText(Pgen))
//									->Parent()->Parent()
//							->LinkEndChild(new TiXmlElement("min-value"))
//								->LinkEndChild(new TiXmlText("0"))
//									->Parent()->Parent()
//							->LinkEndChild(new TiXmlElement("max-value"))
//								->LinkEndChild(new TiXmlText("0"))
//									->Parent()->Parent()->Parent()
//						->LinkEndChild(new TiXmlElement("reactive"))
//							->LinkEndChild(new TiXmlElement("value"))
//								->LinkEndChild(new TiXmlText(Qgen))
//									->Parent()->Parent()
//							->LinkEndChild(new TiXmlElement("min-value"))
//								->LinkEndChild(new TiXmlText(Qmin))
//									->Parent()->Parent()
//							->LinkEndChild(new TiXmlElement("max-value"))
//								->LinkEndChild(new TiXmlText(Qmax))
//								;
//
//			element->LinkEndChild(new TiXmlElement("cost"))
//						->LinkEndChild(new TiXmlElement("part"))
//							->LinkEndChild(new TiXmlElement("power"))
//								->LinkEndChild(new TiXmlElement("min-value"))
//									->LinkEndChild(new TiXmlText("0"))
//									->Parent()->Parent()
//								->LinkEndChild(new TiXmlElement("max-value"))
//									->LinkEndChild(new TiXmlText("0"))
//									->Parent()->Parent()->Parent()
//							->LinkEndChild(new TiXmlElement("coefs"))
//								->LinkEndChild(new TiXmlElement("A"))
//									->LinkEndChild(new TiXmlText("0"))
//									->Parent()->Parent()
//								->LinkEndChild(new TiXmlElement("B"))
//									->LinkEndChild(new TiXmlText("0"))
//									->Parent()->Parent()
//								->LinkEndChild(new TiXmlElement("C"))
//									->LinkEndChild(new TiXmlText("0"))
//									->Parent()->Parent()->Parent()->Parent()
//						->LinkEndChild(new TiXmlElement("part"))
//							->LinkEndChild(new TiXmlElement("power"))
//								->LinkEndChild(new TiXmlElement("min-value"))
//									->LinkEndChild(new TiXmlText("0"))
//									->Parent()->Parent()
//								->LinkEndChild(new TiXmlElement("max-value"))
//									->LinkEndChild(new TiXmlText("0"))
//									->Parent()->Parent()->Parent()
//							->LinkEndChild(new TiXmlElement("coefs"))
//								->LinkEndChild(new TiXmlElement("A"))
//									->LinkEndChild(new TiXmlText("0"))
//									->Parent()->Parent()
//								->LinkEndChild(new TiXmlElement("B"))
//									->LinkEndChild(new TiXmlText("0"))
//									->Parent()->Parent()
//								->LinkEndChild(new TiXmlElement("C"))
//									->LinkEndChild(new TiXmlText("0"))
//									->Parent()->Parent()->Parent()->Parent()
//						->LinkEndChild(new TiXmlElement("part"))
//							->LinkEndChild(new TiXmlElement("power"))
//								->LinkEndChild(new TiXmlElement("min-value"))
//									->LinkEndChild(new TiXmlText("0"))
//									->Parent()->Parent()
//								->LinkEndChild(new TiXmlElement("max-value"))
//									->LinkEndChild(new TiXmlText("0"))
//									->Parent()->Parent()->Parent()
//							->LinkEndChild(new TiXmlElement("coefs"))
//								->LinkEndChild(new TiXmlElement("A"))
//									->LinkEndChild(new TiXmlText("0"))
//									->Parent()->Parent()
//								->LinkEndChild(new TiXmlElement("B"))
//									->LinkEndChild(new TiXmlText("0"))
//									->Parent()->Parent()
//								->LinkEndChild(new TiXmlElement("C"))
//									->LinkEndChild(new TiXmlText("0"))
//									->Parent()->Parent()->Parent()->Parent()
//								;
//		}
//
//		element->LinkEndChild(new TiXmlElement("injected"))->LinkEndChild(new TiXmlText(I));
//
//		buses->LinkEndChild(element);
//	}
//
//	bus_data.SaveFile("bus_data.xml");
}

