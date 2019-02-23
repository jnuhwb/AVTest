//
//  ViewController.m
//  AVTest
//
//  Created by apple on 2019/2/17.
//  Copyright © 2019年 com.hwb. All rights reserved.
//

#import "ViewController.h"
#import "video_converter.h"
#import "simplest_h264_parser.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    UIButton *btn = [[UIButton alloc] init];
    [self.view addSubview:btn];
    btn.translatesAutoresizingMaskIntoConstraints = NO;
    [self.view addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"H:|-30-[v(100)]" options:0 metrics:nil views:@{@"v": btn}]];
    [self.view addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"V:|-30-[v(50)]" options:0 metrics:nil views:@{@"v": btn}]];
    [btn setTitle:@"do it" forState:UIControlStateNormal];
    [btn setBackgroundColor:[UIColor grayColor]];
    [btn addTarget:self action:@selector(btnClicked) forControlEvents:UIControlEventTouchUpInside];
}

- (void)btnClicked {
    [self convertYUV];
    //    [self parseH264];
    //    test();
}

- (void)convertYUV {
    NSString *path = [[NSBundle mainBundle] pathForResource:@"a" ofType:@"mp4"];
    NSString *videoPath = [[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject] stringByAppendingPathComponent:@"video.yuv"];
    NSLog(@"===%@", videoPath);
    int ret = convert2yuv([path cStringUsingEncoding:NSUTF8StringEncoding], [videoPath cStringUsingEncoding:NSUTF8StringEncoding]);
    NSLog(@"===ret=%d", ret);
}

- (void)parseH264 {
    NSString *path = [[NSBundle mainBundle] pathForResource:@"a" ofType:@"h264"];
    simplest_h264_parser([path cStringUsingEncoding:NSUTF8StringEncoding]);
}
@end
