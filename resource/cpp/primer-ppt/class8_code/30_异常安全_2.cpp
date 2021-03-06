#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
using namespace std;

class My_lock_guard {
public:
	explicit My_lock_guard(std::mutex& _mtx):mtx(_mtx){
		mtx.lock(); //构造完成时,资源已经可以使用
	}
	~My_lock_guard() { mtx.unlock(); } //析构时释放
private:
	std::mutex& mtx; //引用
};

class Image {
public:
	Image(int i) :data(i) {
		if (i % 4 == 1) throw "Cr_Img error";
		//1,5,9...抛异常
	}
private:
	int data;
};

class ImgMenu {
public:
	ImgMenu(int i = 0)
		:img(new Image(i)),img_changes(0) {}
	~ImgMenu() { delete img; }
	void set_img(int image); //更改图片
private:
	std::mutex mtx; //互斥体
	Image *img;
	int img_changes; //图片变换的次数
};

void ImgMenu::set_img(int image) {
	//std::lock_guard<std::mutex> lck(mtx);
	My_lock_guard lck(mtx); //代替了1.2
	//1. mtx.lock();  //加锁,申请资源
	delete img;	img = nullptr;
	img_changes++;
	img = new Image(image);
	cout << img_changes << endl;
	//2. mtx.unlock(); //解锁,释放资源
}

ImgMenu img_menu;
void do_thread(int image) { //线程函数
	try {
		img_menu.set_img(image);
	}
	catch (const char * e) {
		cout << e << endl;
	}
}

int main() {
	std::thread threads[8];
	for (int i = 0; i < 8; i++)
		threads[i] = std::thread(do_thread, i);
	for (auto &th : threads) th.join();
	return 0;
}