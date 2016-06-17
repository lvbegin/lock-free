/* Copyright 2016 Laurent Van Begin
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
  */

#include <stack.h>
#include <list.h>

#include <iostream>
#include <thread>


static const unsigned int nbMessage = 100000;
static const unsigned int nbThreads = 10;

void readerThread(lockFree::stack<int> *s)
{
	unsigned int j = 0;
	unsigned int i = 0;
	while (i < nbMessage) {
		try {
			j += s->pop();
			i++;
			if (j % 5 == 0)
				std::cout << "." << std::flush;
		}
		catch (std::exception &e) {
			std::cout << "*" << std::flush;
		}
	}
	std::cout << std::endl;
}

void writerThread(lockFree::stack<int> *s)
{
	for (unsigned int i = 0; i < nbMessage; i++) {
		s->push(1);
		if (i % 5 == 0)
			std::cout << "+" << std::flush;
	}
}

static void test_lock_free_stack(void)
{
	lockFree::stack<int> s;
	std::unique_ptr<std::thread> writer[nbThreads];
	std::unique_ptr<std::thread> reader[nbThreads];

	std::cout << "test lock free stack: ";

	for (unsigned int i = 0; i < nbThreads; i++) {
		writer[i].reset(new std::thread(writerThread, &s));
		reader[i].reset(new std::thread(readerThread, &s));
	}

	for (unsigned int i = 0; i < nbThreads; i++) {
		writer[i]->join();
		reader[i]->join();
	}

	if (s.isEmpty())
		std::cout << "OK" << std::endl;
	else
		std::cout << "KO" << std::endl;


}



void readerThreadList(lockFree::list2<int> *s)
{
	unsigned int j = 0;
	unsigned int i = 0;
	while (i < nbMessage) {
		try {
			j += s->remove();
			i++;
			if (j % 5 == 0)
				std::cout << "." << std::flush;
		}
		catch (std::exception &e) {
			std::cout << "*" << std::flush;
		}
	}
	std::cout << "end reader" << std::endl;
	std::cout << std::endl;
}

void writerThreadList(lockFree::list2<int> *s)
{
	for (unsigned int i = 0; i < nbMessage; i++) {
		s->insert(1);
		if (i % 5 == 0)
			std::cout << "+" << std::flush;
	}
}

static void test_lock_free_list(void)
{
	lockFree::list2<int> s;
	std::unique_ptr<std::thread> writer[nbThreads];
	std::unique_ptr<std::thread> reader[nbThreads];

	std::cout << "test lock free stack: ";

	for (unsigned int i = 0; i < nbThreads; i++) {
		writer[i].reset(new std::thread(writerThreadList, &s));
		reader[i].reset(new std::thread(readerThreadList, &s));
	}
	for (unsigned int i = 0; i < nbThreads; i++) {
		writer[i]->join();
		reader[i]->join();
	}

	if (s.isEmpty())
		std::cout << "OK" << std::endl;
	else
		std::cout << "KO" << std::endl;
}
//
//
//static void test_lock_free_list(void) {
//	lockFree::list<int> l;
//	l.insert(3);
//	auto v = l.remove();
//	std::cout << v << std::endl;
//
//	lockFree::list2<int> l2;
//	l2.insert(4);
//	l2.insert(5);
//
//	auto v2 = l2.remove();
//	std::cout << v2 << std::endl;
//	v2 = l2.remove();
//	std::cout << v2 << std::endl;
//
//}

int main() {
	test_lock_free_stack();
	test_lock_free_list();
	return EXIT_SUCCESS;
}
