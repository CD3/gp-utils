../README.md: README.md.t help-messages.txt
	expand-macros.py $< $@

help-messages.txt:generate-usage-for-docs.sh
	./generate-usage-for-docs.sh
