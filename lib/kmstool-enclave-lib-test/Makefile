build:
	./build.sh

docker:
	docker build -t kmstool-test -f ./container/Dockerfile .

eif:
	nitro-cli build-enclave --docker-uri kmstool-test --output-file kmstool-test.eif

test:
	nitro-cli run-enclave \
		--enclave-name kmstool-test \
		--enclave-cid 1001 \
		--eif-path ./kmstool-test.eif \
		--cpu-count 2 \
		--memory 1024 \
		--attach-console \
		--debug-mode

stop:
	nitro-cli terminate-enclave --all

.PHONY: clean
clean:
	rm -rf libnsm.so
	rm -rf libkmstool-enclave-lib.so
	rm -rf kmstool_enclave_lib_test
	rm -rf kmstool-test.eif

